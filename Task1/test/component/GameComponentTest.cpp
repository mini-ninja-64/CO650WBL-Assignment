#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "CommonTests.hpp"
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

TEST_UPDATE_FUNCTION_LOGS_ID_AND_TIMESTAMP(GameComponentTest, GameComponent)