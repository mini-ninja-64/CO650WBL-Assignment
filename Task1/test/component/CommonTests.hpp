#pragma once

#include <chrono>
#include <string>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


#define TEST_UPDATE_FUNCTION_LOGS_ID_AND_TIMESTAMP(TEST_FIXTURE,               \
                                                   CONSTRUCTOR_TYPE, ...)      \
  TEST(TEST_FIXTURE, LogsIdAndTimestampWhenUpdateIsCalled) {                   \
    auto gameComponent = std::make_unique<CONSTRUCTOR_TYPE>(__VA_ARGS__);      \
                                                                               \
    std::chrono::time_point<std::chrono::system_clock> timePoint;              \
    auto cTime = std::chrono::system_clock::to_time_t(timePoint);              \
    auto *localTime = std::localtime(&cTime);                                  \
                                                                               \
    testing::internal::CaptureStdout();                                        \
    gameComponent->update(localTime);                                          \
    std::string output = testing::internal::GetCapturedStdout();               \
    EXPECT_THAT(output,                                                        \
                testing::ContainsRegex("ID: [0-9]+ Updated @ 01:00:00\n"));    \
                                                                               \
    testing::internal::CaptureStdout();                                        \
    timePoint += std::chrono::minutes(45);                                     \
    cTime = std::chrono::system_clock::to_time_t(timePoint);                   \
    localTime = std::localtime(&cTime);                                        \
    gameComponent->update(localTime);                                          \
    output = testing::internal::GetCapturedStdout();                           \
    EXPECT_THAT(output,                                                        \
                testing::ContainsRegex("ID: [0-9]+ Updated @ 01:45:00\n"));    \
  }
