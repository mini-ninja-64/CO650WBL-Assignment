#pragma once

#include <span>
#include <cstdlib>

#include "../Packet.hpp"

class PacketHeaderV1: public PacketHeader {
public:
    explicit PacketHeaderV1(uint16_t senderId);
    ~PacketHeaderV1() override = default;

    std::vector<uint8_t> getBytes() override;

    [[nodiscard]] uint16_t getSenderId() const;

private:
    const uint16_t senderId;
};
