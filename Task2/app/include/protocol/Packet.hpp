#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <memory>

#include "PacketBody.hpp"
#include "PacketHeader.hpp"
#include "Serializable.hpp"

class Packet: public Serializable {
public:
    Packet(uint8_t version,
           std::unique_ptr<PacketHeader> packetHeader,
           std::unique_ptr<PacketBody> packetBody);
    virtual ~Packet() = default;

    [[nodiscard]] std::vector<uint8_t> getBytes() override;

    [[nodiscard]] uint8_t getVersion() const;
    [[nodiscard]] const std::unique_ptr<PacketHeader> &getPacketHeader() const;
    [[nodiscard]] const std::unique_ptr<PacketBody> &getPacketBody() const;

    [[nodiscard]] static std::unique_ptr<Packet> parsePacket(std::span<uint8_t> bytes);

private:
    const uint8_t version;
    std::unique_ptr<PacketHeader> packetHeader;
    std::unique_ptr<PacketBody> packetBody;
};
