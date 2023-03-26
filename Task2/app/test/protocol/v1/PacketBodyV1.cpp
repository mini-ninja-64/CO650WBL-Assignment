#include "protocol/v1/PacketBodyV1.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(PacketBodyV1, CorrectlyGeneratesBytesFromAsciiPacketBody) {
    std::array<uint8_t, 5> bodyData = { 'h', 'e', 'l', 'l', 'o' };
    AsciiPacketBodyV1 asciiPacketBodyV1(bodyData);

    std::vector<uint8_t> expectedBytes = {
            0x01, // body type
            0x00, 0x00, 0x00, 0x05, // body length
            0x68, 0x65, 0x6C, 0x6C, 0x6F // body content
    };
    ASSERT_EQ(asciiPacketBodyV1.getBytes(), expectedBytes);
}

TEST(PacketBodyV1, CorrectlyGeneratesStringsFromAsciiPacketBody) {
    std::array<uint8_t, 5> bodyData = { 'h', 'e', 'l', 'l', 'o' };
    AsciiPacketBodyV1 asciiPacketBodyV1(bodyData);

    ASSERT_EQ(asciiPacketBodyV1.asString(), "hello");
}

TEST(PacketBodyV1, CorrectlyGeneratesBytesFromQuitPacketBody) {
    QuitPacketBodyV1 quitPacketBodyV1;

    std::vector<uint8_t> expectedBytes = {
            0x02, // body type
            0x00, 0x00, 0x00, 0x00, // body length
    };
    ASSERT_EQ(quitPacketBodyV1.getBytes(), expectedBytes);
}
