#include "protocol/Packet.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockPacketHeader : public PacketHeader {
public:
    MOCK_METHOD(std::vector<uint8_t>, getBytes, (), (override));
};

class MockPacketBody : public PacketBody {
public:
    MOCK_METHOD(std::vector<uint8_t>, getBytes, (), (override));
};

TEST(Packet, CombinesPacketVersionHeaderAndBodyWhenGetBytesIsCalled) {
    auto mockPacketHeader = std::make_unique<MockPacketHeader>();
    EXPECT_CALL(*mockPacketHeader, getBytes())
            .Times(1)
            .WillRepeatedly(testing::Return(std::vector<uint8_t>({0xBB,0xCC})));

    auto mockPacketBody = std::make_unique<MockPacketBody>();
    EXPECT_CALL(*mockPacketBody, getBytes())
            .Times(1)
            .WillRepeatedly(testing::Return(std::vector<uint8_t>({0xDD,0xEE})));

    Packet packet(0xAA, std::move(mockPacketHeader), std::move(mockPacketBody));
    ASSERT_EQ(packet.getBytes(), std::vector<uint8_t>({0xAA,0xBB,0xCC,0xDD,0xEE}));
}

TEST(Packet, ThrowsExceptionWhenAttemptingToParseUnsupportedVersion) {
    const std::vector<uint8_t> unsupportedVersions = {0x00, 0x02, 0x03, 0x04, 0x05, 0x06, 0xFF};

    for (const auto &unsupportedVersion: unsupportedVersions) {
        std::array<uint8_t, 1> packetData = { unsupportedVersion };
        EXPECT_THROW({
            auto packet = Packet::parsePacket(packetData);
        }, std::invalid_argument);
    }
}

TEST(Packet, ThrowsExceptionWhenPacketIsNotAMinimumOf1Byte) {
        std::array<uint8_t, 0> packetData = { };
        EXPECT_THROW({
            auto packet = Packet::parsePacket(packetData);
        }, std::length_error);
}
