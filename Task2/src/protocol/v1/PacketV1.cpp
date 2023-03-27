#include "protocol/v1/PacketV1.hpp"

#include <utility>

#define PACKET_VERSION 1

PacketV1::PacketV1(std::unique_ptr<PacketHeaderV1> packetHeader, std::unique_ptr<PacketBodyV1> packetBody):
        Packet(PACKET_VERSION, std::move(packetHeader), std::move(packetBody)) {}

std::vector<uint8_t> PacketV1::getBytes() {
    return Packet::getBytes();
}

std::unique_ptr<PacketV1> PacketV1::parsePacket(const std::span<uint8_t> bytes) {
    // Version (1) + Sender ID (2) + Body Type (1) + Body Length (4)
    const static uint8_t MINIMUM_PACKET_SIZE = 8;
    if(bytes.size() < MINIMUM_PACKET_SIZE)
        throw std::length_error("A v1 packet must be at least 8 bytes long");

    // Sanity check version of packet
    const auto packetVersion = bytes[0];
    if(packetVersion != PACKET_VERSION)
        throw std::invalid_argument("Unexpected version of packet found when attempting to parse v1 packet");

    // Parse v1 packet header
    const uint16_t senderId = static_cast<uint16_t>(bytes[1]) << 8 | static_cast<uint16_t>(bytes[2]);
    auto packetHeader = std::make_unique<PacketHeaderV1>(senderId);

    // Parse v1 packet body
    const uint8_t bodyTypeByte = bytes[3];
    const auto bodyType = static_cast<PacketBodyV1::BodyType>(bodyTypeByte);

    const uint32_t bodyLength = static_cast<uint32_t>(bytes[4]) << 8*3 |
                                        static_cast<uint32_t>(bytes[5]) << 8*2 |
                                        static_cast<uint32_t>(bytes[6]) << 8*1 |
                                        static_cast<uint32_t>(bytes[7]);

    uint32_t remainingPacketLength = bytes.size() - 7;

    if(bodyLength > remainingPacketLength)
        throw std::length_error("Body length reported by packet exceeds available bytes");

    std::vector<uint8_t> bodyData(bytes.begin()+8, bytes.begin()+8+bodyLength);

    std::unique_ptr<PacketBodyV1> packetBody = nullptr;
    switch (bodyType) {
        case PacketBodyV1::QUIT:
            packetBody = std::make_unique<QuitPacketBodyV1>();
            break;

        case PacketBodyV1::ASCII:
            packetBody = std::make_unique<AsciiPacketBodyV1>(bodyData);
            break;
        default:
            throw std::invalid_argument("Found unsupported body type in v1 packet");
    }

    return std::make_unique<PacketV1>(std::move(packetHeader), std::move(packetBody));
}
