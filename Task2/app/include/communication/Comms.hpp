#pragma once

#include <span>
#include <functional>

#include "protocol/Packet.hpp"

#define POLL_TIMEOUT_MS 1000

class Comms;
typedef std::function<void(const std::unique_ptr<Packet>&)> MessageHandler;

class Comms {
public:
    Comms(int port, MessageHandler messageHandler);
    virtual ~Comms() = default;

    virtual void sendPacket(std::unique_ptr<Packet> packet) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

    [[nodiscard]] virtual bool isRunning() const = 0;

    [[nodiscard]] int getPort() const;
    [[nodiscard]] const MessageHandler &getMessageHandler() const;
private:
    const int port;
    const MessageHandler messageHandler;
};
