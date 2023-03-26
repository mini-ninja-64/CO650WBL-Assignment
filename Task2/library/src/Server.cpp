#include <cerrno>
#include <poll.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <utility>
#include <unordered_set>
#include <vector>
#include <array>
#include <sys/fcntl.h>
#include "Server.hpp"
//#include "utils.hpp"

#define SERVER_ADDRESS INADDR_ANY

#define SOCKET_DOMAIN AF_INET
#define SOCKET_TYPE SOCK_STREAM
#define SOCKET_PROTOCOL 0 //default protocol

#define ERROR_LOG std::strerror(errno) << " [" << errno << "]"

static void newConnectionListenerThreadFunction(int socketDescriptor,
                                                const bool& running,
                                                std::mutex& connectionListMutex,
                                                std::unordered_set<int>& connectionList) {
    while (running) {
        // Blocking wait for events on socket descriptor
        int connectionDescriptor = accept(socketDescriptor, nullptr, nullptr);
        if(connectionDescriptor < 0) {
            std::cerr << "Failed to accept inbound connection:" << ERROR_LOG << std::endl;
        } else {
            std::cout << "Registering New connection: " << connectionDescriptor << std::endl;
            std::cout << "Configuring connection to be non-blocking" << std::endl;
            int currentFlags = fcntl(connectionDescriptor, F_GETFL, 0);
            fcntl(connectionDescriptor, F_SETFL,  currentFlags | O_NONBLOCK);

            std::lock_guard<std::mutex> connectionListLock(connectionListMutex);
            connectionList.insert(connectionDescriptor);
        }
    }
}

static void currentConnectionHandlerThreadFunction(const bool& running,
                                                   std::mutex& connectionListMutex,
                                                   std::unordered_set<int>& connectionList,
                                                   const ConnectionHandler& connectionHandler) {

    auto createPollDescriptor = [](auto connectionDescriptor) {
        return pollfd{
                .fd = connectionDescriptor,
                .events = POLLIN | POLLOUT,
                .revents = 0
        };
    };
    while (running) {
        std::vector<pollfd> pollDescriptors(connectionList.size());

        {
            std::lock_guard<std::mutex> connectionListLock(connectionListMutex);
            std::transform(connectionList.begin(), connectionList.end(), pollDescriptors.begin(), createPollDescriptor);
        }

        poll(pollDescriptors.data(), pollDescriptors.size(), 100);

        std::unordered_set<int> closeableConnections;
        for (const auto &pollDescriptor: pollDescriptors) {
            if(pollDescriptor.revents & POLLHUP) {
                std::cerr << "Connection hung up: " << pollDescriptor.fd << ", " << ERROR_LOG << std::endl;
                closeableConnections.insert(pollDescriptor.fd);
                break;
            }

            // TODO: Should decouple these 2 events and instead push write data into memory until socket is writeable
            //       to read as fast as possible.
            if (pollDescriptor.revents & POLLIN &&
                pollDescriptor.revents & POLLOUT) {
                std::cout << "New data to read on connection: " << pollDescriptor.fd << std::endl;
                const static size_t READ_STACK_SIZE = 1024;
                std::array<uint8_t, READ_STACK_SIZE> readArray = {0};

                auto bytesRead = read(pollDescriptor.fd, static_cast<void*>(readArray.data()), READ_STACK_SIZE);

                if (bytesRead < 0)
                    std::cerr << "Error occurred when trying to read connection "
                              << pollDescriptor.fd
                              << ", " << ERROR_LOG
                              << std::endl;

                if (bytesRead > 0) {
                    std::cout << "Received " << bytesRead << " bytes from connection: " << pollDescriptor.fd << std::endl;

                    std::vector<uint8_t > readVector(readArray.begin(), readArray.begin() + bytesRead);
                    auto dataToWrite = connectionHandler(readVector);

                    auto writeResult = write(pollDescriptor.fd, dataToWrite.data(), dataToWrite.size());
                    if(writeResult == -1) {
                        std::cerr << "Failed to write to connection: " << pollDescriptor.fd << ", " << ERROR_LOG << std::endl;
                        closeableConnections.insert(pollDescriptor.fd);
                    }
                }
            }
        }

        if(!closeableConnections.empty()) {
            {
                // take exclusive access to drain the connectionList of soon-to-be closed connections
                std::lock_guard<std::mutex> connectionListLock(connectionListMutex);
                for (const auto &connectionDescriptor: closeableConnections) {
                    connectionList.erase(connectionDescriptor);
                }
            }
            for (const auto &connectionDescriptor: closeableConnections) {
                std::cout << "Closing connection: " << connectionDescriptor << std::endl;
                if(close(connectionDescriptor)) {
                    std::cerr << "Failed to close connection: "
                              << connectionDescriptor
                              << ", " << ERROR_LOG
                              << std::endl;
                };
            }
        }
    }
}


SocketServer::SocketServer(int port, ConnectionHandler connectionHandler)
        : connectionHandler(std::move(connectionHandler)),
          socketDescriptor(-1),
          socketAddress({.sin_family = SOCKET_DOMAIN,
                                .sin_port = htons(port),
                                .sin_addr = {.s_addr = htonl(SERVER_ADDRESS)},
                                .sin_zero = {0, 0, 0, 0, 0, 0, 0, 0}}) {}

int SocketServer::initialise() {
    if (initialised) return 0;
    std::cout << "Attempting to initialise server" << std::endl;

    std::cout << "Creating Socket" << std::endl;
    socketDescriptor = socket(SOCKET_DOMAIN, SOCKET_TYPE, SOCKET_PROTOCOL);
    if (socketDescriptor < 0) return errno;

//   Would love a native defer statement to avoid this
#define RETURN_ERRNO_AND_CLOSE_ON_FAILURE(status) if(status) {int errorCopy = errno; close(socketDescriptor); return errorCopy;}

    std::cout << "Binding socket to address" << std::endl;
    RETURN_ERRNO_AND_CLOSE_ON_FAILURE(
            bind(socketDescriptor, reinterpret_cast<sockaddr *>(&socketAddress), sizeof(socketAddress))
    );

    std::cout << "Enabling listening on socket" << std::endl;
    RETURN_ERRNO_AND_CLOSE_ON_FAILURE(
            listen(socketDescriptor, 10)
    );

    initialised = true;
    return 0;
}

int SocketServer::start() {
    if (running) return 0;

    int initialise_failed = initialise();
    if (initialise_failed) return initialise_failed;

    running = true;
    newConnectionListener =  std::thread(newConnectionListenerThreadFunction, socketDescriptor, running, std::ref(connectionListMutex), std::ref(connectionList));
    currentConnectionHandler = std::thread(currentConnectionHandlerThreadFunction, running, std::ref(connectionListMutex), std::ref(connectionList), std::ref(connectionHandler));

    std::cout << "Server ready and running" << std::endl;
    return 0;
}

int SocketServer::stop() {
    if (!running) return 0;

    int initialise_failed = initialise();
    if (initialise_failed) return initialise_failed;

    running = false;
    block();

    // Close all connections
    for (const auto &connectionDescriptor: connectionList) {
        close(connectionDescriptor);
    }

    initialised = false;
    newConnectionListener = std::thread();
    currentConnectionHandler = std::thread();
    if(close(socketDescriptor)) {
        std::cerr << "Failed to close socket: " << ERROR_LOG << std::endl;
        return errno;
    }

    return 0;
}

SocketServer::~SocketServer() {
    if(stop() != 0) {
        //TODO: throw error
    }
}

void SocketServer::block() {
    if(!running) return;
    currentConnectionHandler.join();
    newConnectionListener.join();
}


