#pragma once

#include <span>
#include <functional>

#include "protocol/v1/PacketV1.hpp"

class Comms {
public:
    explicit Comms(int port);

    virtual void sendMessage() = 0;
private:
    int port;
    std::function<void(const Comms&, PacketV1)> messageHandler;
};
