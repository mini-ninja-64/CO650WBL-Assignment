#pragma once

#include <chrono>
#include <string>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


#define TEST_UPDATE_FUNCTION_LOGS_ID_AND_TIMESTAMP(TEST_FIXTURE, CONSTRUCTOR_TYPE, ...) \
TEST(TEST_FIXTURE, LogsIdAndTimestampWhenUpdateIsCalled) { \
    auto gameComponent = std::make_unique<CONSTRUCTOR_TYPE>(__VA_ARGS__); \
    std::chrono::time_point<std::chrono::system_clock> epochTime; \
    \
    testing::internal::CaptureStdout(); \
    gameComponent->update(epochTime); \
    std::string output = testing::internal::GetCapturedStdout(); \
    EXPECT_THAT(output, testing::ContainsRegex("ID: [0-9]+ Updated @ 01:00:00\n")); \
    \
    testing::internal::CaptureStdout(); \
    gameComponent->update(epochTime+std::chrono::minutes(45)); \
    output = testing::internal::GetCapturedStdout(); \
    EXPECT_THAT(output, testing::ContainsRegex("ID: [0-9]+ Updated @ 01:45:00\n")); \
}
