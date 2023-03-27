#pragma once

#include <netinet/in.h>
#include <thread>
#include <unordered_set>
#include "Comms.hpp"

class Server: public Comms {
public:
    Server(int port, MessageHandler messageHandler);
    ~Server() override;

    void sendPacket(std::unique_ptr<Packet> packet) override;

    void block() override;
    void stop() override;

    [[nodiscard]] bool isRunning() const override;

private:
    sockaddr_in serverAddress;
    int socketDescriptor;

    std::mutex packetQueueMutex;
    std::vector<std::unique_ptr<Packet>> packetQueue;

    std::mutex connectionListMutex;
    std::unordered_set<int> connectionList;


    std::mutex runningMutex;
    bool running = false;

    std::thread messageHandlerThread;
    std::thread connectionHandlerThread;

    void setRunningFalse();
    void setRunningFalseAndWaitForCompletion();
};

