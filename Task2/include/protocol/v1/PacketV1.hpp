#pragma once

#include <span>
#include <cstdlib>

#include "../Packet.hpp"
#include "PacketHeaderV1.hpp"
#include "PacketBodyV1.hpp"

class PacketV1: public Packet {
public:
    PacketV1(std::unique_ptr<PacketHeaderV1> packetHeader, std::unique_ptr<PacketBodyV1> packetBody);
    ~PacketV1() override = default;

    std::vector<uint8_t> getBytes() override;

    static std::unique_ptr<PacketV1> parsePacket(std::span<uint8_t> bytes);
};
