#include "protocol/v1/PacketBodyV1.hpp"

#include <string>

PacketBodyV1::PacketBodyV1(PacketBodyV1::BodyType bodyType): bodyType(bodyType) {}

PacketBodyV1::BodyType PacketBodyV1::getBodyType() const {
    return bodyType;
}

#define UINT32_TO_UINT8(value) static_cast<uint8_t>((value & 0xFF000000) >> 8*3), \
                               static_cast<uint8_t>((value & 0x00FF0000) >> 8*2), \
                               static_cast<uint8_t>((value & 0x0000FF00) >> 8*1), \
                               static_cast<uint8_t>((value & 0x000000FF))

AsciiPacketBodyV1::AsciiPacketBodyV1(std::span<uint8_t> bodyData):
    PacketBodyV1(ASCII),
    bodyData(bodyData.begin(), bodyData.end()) {}

AsciiPacketBodyV1::AsciiPacketBodyV1(const std::string &bodyString):
        PacketBodyV1(ASCII),
        bodyData(bodyString.begin(), bodyString.end()) {}

std::vector<uint8_t> AsciiPacketBodyV1::getBytes() {
    std::vector<uint8_t> bodyBytes = {ASCII, UINT32_TO_UINT8(bodyData.size())};

    bodyBytes.insert(bodyBytes.end(), bodyData.begin(), bodyData.end());
    return bodyBytes;
}

std::string AsciiPacketBodyV1::asString() const {
    return { bodyData.begin(), bodyData.end() };
}

QuitPacketBodyV1::QuitPacketBodyV1() :
        PacketBodyV1(QUIT) {}

std::vector<uint8_t> QuitPacketBodyV1::getBytes() {
    return {QUIT, 0, 0, 0, 0};
}

