#include <stdexcept>

#include "protocol/Packet.hpp"
#include "protocol/v1/PacketV1.hpp"

Packet::Packet(uint8_t version,
               std::unique_ptr<PacketHeader> packetHeader,
               std::unique_ptr<PacketBody> packetBody):
                       version(version),
                       packetHeader(std::move(packetHeader)),
                       packetBody(std::move(packetBody)) {}

uint8_t Packet::getVersion() const {
    return version;
}

std::vector<uint8_t> Packet::getBytes() {
    std::vector<uint8_t> packetBytes = {version};

    const auto headerBytes = packetHeader->getBytes();
    packetBytes.insert(packetBytes.end(),
                       std::make_move_iterator(headerBytes.begin()),
                       std::make_move_iterator(headerBytes.end())
    );

    const auto bodyBytes = packetBody->getBytes();
    packetBytes.insert(packetBytes.end(),
                       std::make_move_iterator(bodyBytes.begin()),
                       std::make_move_iterator(bodyBytes.end())
    );

    return packetBytes;
}

std::unique_ptr<Packet> Packet::parsePacket(std::span<uint8_t> bytes) {
     if(bytes.empty()) throw std::length_error("Packet must have at least 1 byte");
    const auto version = bytes[0];

    switch (version) {
        case 0x01:
            return PacketV1::parsePacket(bytes);
        default:
            throw std::invalid_argument("Unsupported version of packet");
    }
}

const std::unique_ptr<PacketHeader> &Packet::getPacketHeader() const {
    return packetHeader;
}

const std::unique_ptr<PacketBody> &Packet::getPacketBody() const {
    return packetBody;
}
