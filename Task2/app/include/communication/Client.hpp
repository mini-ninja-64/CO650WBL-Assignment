#pragma once

#include <netinet/in.h>
#include <thread>
#include <unordered_set>

#include "Comms.hpp"

class Client: public Comms {
public:
    Client(const std::string& address, int port, MessageHandler messageHandler);
    ~Client() override;

    void sendPacket(std::unique_ptr<Packet> packet) override;

    void start() override;
    void stop() override;
    [[nodiscard]] bool isRunning() const override;

private:
    sockaddr_in serverAddress;
    int socketDescriptor;

    std::mutex packetQueueMutex;
    std::vector<std::unique_ptr<Packet>> packetQueue;

    std::mutex runningMutex;
    bool running = false;
    std::thread messageHandlerThread;

    void stopThreadAndCloseSocketDescriptor();
};
