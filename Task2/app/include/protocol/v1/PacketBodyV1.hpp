#pragma once

#include <span>
#include <vector>
#include <vector>
#include <cstdlib>

#include "../Packet.hpp"

class PacketBodyV1: public PacketBody {
public:
    enum BodyType {
        ASCII = 0x01,
        QUIT = 0x02
    };

    explicit PacketBodyV1(BodyType bodyType);
    ~PacketBodyV1() override = default;

    std::vector<uint8_t> getBytes() override = 0;

    [[nodiscard]] BodyType getBodyType() const;
private:
    const BodyType bodyType;
};

class AsciiPacketBodyV1: public PacketBodyV1 {
public:
    explicit AsciiPacketBodyV1(std::span<uint8_t> bodyData);
    explicit AsciiPacketBodyV1(const std::string& bodyString);
    ~AsciiPacketBodyV1() override = default;

    [[nodiscard]] std::vector<uint8_t> getBytes() override;

    [[nodiscard]] std::string asString() const;

private:
    std::vector<uint8_t> bodyData;
};

class QuitPacketBodyV1: public PacketBodyV1 {
public:
    QuitPacketBodyV1();
    ~QuitPacketBodyV1() override = default;

    std::vector<uint8_t> getBytes() override;
};
