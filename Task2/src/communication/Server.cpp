#include "communication/Server.hpp"
#include "util/unix.hpp"
#include "communication/handleSocketEvents.hpp"

#include <utility>
#include <sys/poll.h>
#include <iostream>
#include <sys/fcntl.h>
#include <sstream>
#include <libc.h>

static void connectionHandlerThreadFunction(int socketDescriptor,
                                            const bool& running,
                                            std::mutex& connectionListMutex,
                                            std::unordered_set<int>& connectionList) {
    while (running) {
        // Blocking wait for events on socket descriptor
        int connectionDescriptor = accept(socketDescriptor, nullptr, nullptr);
        if(connectionDescriptor < 0) {
            if(errno == ECONNABORTED && !running) {
                std::cout << "Server socket closed, completing thread" << std::endl;
                break;
            }

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

static void messageHandlerThreadFunction(int serverSocketDescriptor,
                                         std::thread& connectionHandlerThread,
                                         bool& running,
                                         std::unordered_set<int>& connectionList,
                                         std::mutex& connectionListMutex,
                                         std::vector<std::unique_ptr<Packet>>& packetQueue,
                                         std::mutex& packetQueueMutex,
                                         const MessageHandler& messageHandler) {

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

        poll(pollDescriptors.data(), pollDescriptors.size(), POLL_TIMEOUT_MS);

        std::unordered_set<int> closeableConnections;
        for (size_t pollDescriptorIndex = 0; pollDescriptorIndex < pollDescriptors.size(); pollDescriptorIndex++) {
            const auto& pollDescriptor = pollDescriptors[pollDescriptorIndex];
            const bool isFinalDescriptor = pollDescriptorIndex == pollDescriptors.size()-1;

            try {
                handleSocketEvents(pollDescriptor,
                                   isFinalDescriptor,
                                   packetQueueMutex,
                                   packetQueue,
                                   messageHandler
                );
            } catch (const std::exception& exception) {
                if(!running) continue; // if not running, any errors are expected as sockets are being closed

                std::cerr << "Exception occurred while handling events, connection " <<
                    pollDescriptor.fd << " will be closed (exception: " <<
                    exception.what() << ")" << std::endl;

                closeableConnections.insert(pollDescriptor.fd);
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
                    std::cerr << "Failed to stop connection: "
                              << connectionDescriptor
                              << ERROR_LOG
                              << std::endl;
                };
            }
        }
    }

    // Force the server socket to close and wait for the connectionHandler thread to finish up
    if(close(serverSocketDescriptor) < 0) {
        std::cerr << "Failed to close server socket: " << serverSocketDescriptor << ERROR_LOG << std::endl;
    }
    if(connectionHandlerThread.joinable()) connectionHandlerThread.join();

    // Close all connection sockets
    std::lock_guard<std::mutex> connectionListLock(connectionListMutex);
    for (const auto &connectionDescriptor: connectionList) {
        if(close(connectionDescriptor) < 0) {
            std::cerr << "Failed to close connection: " << connectionDescriptor << ERROR_LOG << std::endl;
        }
    }
    connectionList.clear();
}

Server::Server(int port, MessageHandler messageHandler):
        Comms(port, std::move(messageHandler)),
        serverAddress({
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = {
                    // Use INADDR_ANY to bind to all available local addresses
                    // as is customary for server applications.
                    .s_addr = htonl(INADDR_ANY)
            },
            .sin_zero = {0, 0, 0, 0, 0, 0, 0, 0}
        }) {
    std::lock_guard<std::mutex> runningGuard(runningMutex);
    if(running) return;

    std::cout << "Attempting to create blocking client socket descriptor" << std::endl;
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socketDescriptor < 0) {
        std::stringstream errorString;
        errorString << "Failed to create socket descriptor" << ERROR_LOG;
        throw std::runtime_error(errorString.str());
    }

    std::cout << "Attempting to create bind socket to address" << std::endl;
    int socketBinding = bind(
            socketDescriptor,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)
    );
    if(socketBinding < 0) {
        std::stringstream errorString;
        errorString << "Failed to bind socket to server address" << ERROR_LOG;
        close(socketDescriptor);
        throw std::runtime_error(errorString.str());
    }

    std::cout << "Attempting to enable listening on server socket" << std::endl;
    int socketListening = listen(socketDescriptor, 10);
    if(socketListening < 0) {
        std::stringstream errorString;
        errorString << "Failed to enable listening on server socket" << ERROR_LOG;
        close(socketDescriptor);
        throw std::runtime_error(errorString.str());
    }

    running = true;
    connectionHandlerThread = std::thread(connectionHandlerThreadFunction,
                                          socketDescriptor,
                                          std::ref(running),
                                          std::ref(connectionListMutex),
                                          std::ref(connectionList));

    messageHandlerThread = std::thread(messageHandlerThreadFunction,
                                       socketDescriptor,
                                       std::ref(connectionHandlerThread),
                                       std::ref(running),
                                       std::ref(connectionList),
                                       std::ref(connectionListMutex),
                                       std::ref(packetQueue),
                                       std::ref(packetQueueMutex),
                                       getMessageHandler());
}

Server::~Server() {
    setRunningFalseAndWaitForCompletion();
}

void Server::sendPacket(std::unique_ptr<Packet> packet) {
    std::lock_guard<std::mutex> packetQueueGuard(packetQueueMutex);
    packetQueue.push_back(std::move(packet));
}

void Server::stop() {
    setRunningFalse();
}

bool Server::isRunning() const {
    return running;
}

void Server::setRunningFalse() {
    std::lock_guard<std::mutex> runningGuard(runningMutex);

    // Ensure all outstanding packets have been sent before closing the server
    while(!packetQueue.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    if(!running) return;
    running = false;
}


void Server::setRunningFalseAndWaitForCompletion() {
    setRunningFalse();
    block();
}

void Server::block() {
    if(messageHandlerThread.joinable()) messageHandlerThread.join();
    if(connectionHandlerThread.joinable()) connectionHandlerThread.join();
}

