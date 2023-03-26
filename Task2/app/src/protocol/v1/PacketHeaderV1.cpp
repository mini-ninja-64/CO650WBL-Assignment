#include "protocol/v1/PacketHeaderV1.hpp"

#define UINT16_TO_UINT8(value) static_cast<uint8_t>((value & 0xFF00) >> 8), static_cast<uint8_t>(value & 0x00FF)

PacketHeaderV1::PacketHeaderV1(uint16_t senderId): senderId(senderId) {}

std::vector<uint8_t> PacketHeaderV1::getBytes() {
    return { UINT16_TO_UINT8(senderId) };
}

uint16_t PacketHeaderV1::getSenderId() const {
    return senderId;
}
