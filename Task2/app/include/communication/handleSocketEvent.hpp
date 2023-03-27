#pragma once

#include <sys/poll.h>
#include <iostream>
#include <memory>
#include <functional>
#include "protocol/Packet.hpp"

void handleSocketEvent(const pollfd& pollDescriptor,
                       bool drainPacketQueue,
                       std::mutex& packetQueueMutex,
                       std::vector<std::unique_ptr<Packet>>& packetQueue,
                       const std::function<void(std::unique_ptr<Packet>)>& packetCallback);