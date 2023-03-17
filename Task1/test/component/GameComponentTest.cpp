#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "component/GameComponent.hpp"

TEST(GameComponentTest, IncrementsInstancesOnInstantiationAndUsesItForID) {
    GameComponent gameComponent1;
    auto* gameComponent2 = new GameComponent();
    auto gameComponent3 = std::make_unique<GameComponent>();

    EXPECT_EQ(gameComponent1.getId(), 1);
    EXPECT_EQ(gameComponent2->getId(), 2);
    EXPECT_EQ(gameComponent3->getId(), 3);
    EXPECT_EQ(GameComponent::getInstances(), 3);

    delete gameComponent2;
}

TEST(GameComponentTest, LogsIdAndTimestampWhenUpdateIsCalled) {
    GameComponent gameComponent;
    std::chrono::time_point<std::chrono::system_clock> epochTime;

//  TODO: Consider timezones
    testing::internal::CaptureStdout();
    gameComponent.update(epochTime);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::MatchesRegex("ID: [0-9] Updated @ 01:00:00\n"));

    testing::internal::CaptureStdout();
    gameComponent.update(epochTime+std::chrono::minutes(45));
    output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::MatchesRegex("ID: [0-9] Updated @ 01:45:00\n"));
}
