#include "protocol/v1/PacketV1.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(PacketV1, CorrectlyParsesV1PacketsWithASCIIBodies) {
    std::vector<uint8_t> packetData = {
            0x01, // packet version

            // HEADER
            0xBB, 0xCC, // sender ID

            //BODY
            0x01, // body type (ASCII)
            0x00, 0x00, 0x00, 0x0B,
            't', 'e', 's', 't', ' ', 's', 't', 'r', 'i', 'n', 'g'
    };

    const auto parsedPacket = PacketV1::parsePacket(packetData);
    ASSERT_EQ(parsedPacket->getBytes(), packetData);

    auto packetBody = parsedPacket->getPacketBody().get();
    auto* asciiPacketBody = dynamic_cast<AsciiPacketBodyV1*>(packetBody);
    ASSERT_EQ(asciiPacketBody->getBodyType(), PacketBodyV1::ASCII);
    ASSERT_EQ(asciiPacketBody->asString(), "test string");
}

TEST(PacketV1, CorrectlyParsesV1PacketsWithQuitBodies) {
    std::vector<uint8_t> packetData = {
            0x01, // packet version

            // HEADER
            0xBB, 0xCC, // sender ID

            //BODY
            0x02, // body type (ASCII)
            0x00, 0x00, 0x00, 0x00
    };

    const auto parsedPacket = PacketV1::parsePacket(packetData);
    ASSERT_EQ(parsedPacket->getBytes(), packetData);

    auto packetBody = parsedPacket->getPacketBody().get();
    auto* quitPacketBody = dynamic_cast<QuitPacketBodyV1*>(packetBody);
    ASSERT_EQ(quitPacketBody->getBodyType(), PacketBodyV1::QUIT);
}
