#include "communication/Client.hpp"
#include "protocol/v1/PacketBodyV1.hpp"
#include "protocol/v1/PacketHeaderV1.hpp"
#include "communication/Server.hpp"
#include "util/unix.hpp"
#include "communication/handleSocketEvents.hpp"


#include <utility>
#include <libc.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/poll.h>
#include <libc.h>

static void messageHandlerThreadFunction(int socketDescriptor,
                                         bool& running,
                                         std::mutex& runningMutex,
                                         std::vector<std::unique_ptr<Packet>>& packetQueue,
                                         std::mutex& packetQueueMutex,
                                         const MessageHandler& messageHandler) {

    pollfd pollDescriptor{
        .fd = socketDescriptor,
        .events = POLLIN | POLLOUT,
        .revents = 0
    };

    while (running) {
        poll(&pollDescriptor, 1, POLL_TIMEOUT_MS);

        try {
            handleSocketEvents(pollDescriptor,
                               true,
                               packetQueueMutex,
                               packetQueue,
                               messageHandler
            );
        } catch (const std::exception& exception) {
            std::lock_guard<std::mutex> runningGuard(runningMutex);
            if(!running) continue; // if not running, any errors are expected as sockets are being closed

            std::cerr << "Exception occurred while handling events, (exception: " << exception.what() << ")" << std::endl;
            running = false;
        }
    }

    close(socketDescriptor);
}

Client::Client(const std::string& address, int port, MessageHandler messageHandler):
    Comms(port, std::move(messageHandler)),
    serverAddress({
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {
                .s_addr = inet_addr(address.c_str())
        },
        .sin_zero = {0, 0, 0, 0, 0, 0, 0, 0}
    }) {
    std::lock_guard<std::mutex> runningGuard(runningMutex);
    if(running) return;

    std::cout << "Attempting to create blocking client socket descriptor" << std::endl;
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(socketDescriptor < 0) {
        std::cerr << "Failed to create client socket" << std::endl;
        throw std::runtime_error("Failed to create client socket");
    }

    std::cout << "Attempting to connect client socket to server" << std::endl;
    int clientSocketConnection = connect(
            socketDescriptor,
            reinterpret_cast<sockaddr *>(&serverAddress),
            sizeof(serverAddress)
    );

    if(clientSocketConnection < 0) {
        std::cerr << "Failed to connect to server" << ERROR_LOG << std::endl;
        close(socketDescriptor);
        throw std::runtime_error("Failed to connect to the provided server");
    }

    running = true;
    messageHandlerThread = std::thread(messageHandlerThreadFunction,
                                       socketDescriptor,
                                       std::ref(running),
                                       std::ref(runningMutex),
                                       std::ref(packetQueue),
                                       std::ref(packetQueueMutex),
                                       std::ref(getMessageHandler()));
}

void Client::sendPacket(std::unique_ptr<Packet> packet) {
    std::lock_guard<std::mutex> packetQueueGuard(packetQueueMutex);
    packetQueue.push_back(std::move(packet));
}

Client::~Client() {
    setRunningFalseAndWaitForCompletion();
}

void Client::stop() {
    setRunningFalse();
}

void Client::setRunningFalseAndWaitForCompletion() {
    setRunningFalse();
    block();
}

bool Client::isRunning() const {
    return running;
}

void Client::block() {
    if(messageHandlerThread.joinable()) messageHandlerThread.join();
}

void Client::setRunningFalse() {
    std::lock_guard<std::mutex> runningGuard(runningMutex);

    // Ensure all outstanding packets have been sent before closing the server
    while(!packetQueue.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    if(!running) return;
    running = false;
}

