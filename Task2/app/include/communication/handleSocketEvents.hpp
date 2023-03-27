#pragma once

#include <sys/poll.h>
#include <iostream>
#include <memory>
#include <functional>
#include "protocol/Packet.hpp"
#include "Comms.hpp"

void handleSocketEvents(const pollfd& pollDescriptor,
                        bool drainPacketQueue,
                        std::mutex& packetQueueMutex,
                        std::vector<std::unique_ptr<Packet>>& packetQueue,
                        const MessageHandler& messageHandler);