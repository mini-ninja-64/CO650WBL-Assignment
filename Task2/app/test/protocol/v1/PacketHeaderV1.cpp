#include "protocol/v1/PacketHeaderV1.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(PacketHeaderV1, CorrectlyGeneratesBytesFromObject) {
    PacketHeaderV1 packetHeaderV1(0xAABB);

    std::vector<uint8_t> expectedBytes = {0xAA, 0xBB};
    ASSERT_EQ(packetHeaderV1.getBytes(), expectedBytes);
}
