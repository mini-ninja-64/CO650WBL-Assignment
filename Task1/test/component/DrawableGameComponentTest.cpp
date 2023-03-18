#include <array>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "component/DrawableGameComponent.hpp"
#include "CommonTests.hpp"

// TODO: Test what happens setDirection is called with an illegal direction
// TODO: Test what happens when constructed with x & y outside of legal bounds and decide how it should be handled

#define ASSERT_POS(object, x, y) ASSERT_EQ(object.getX(), x); ASSERT_EQ(object.getY(), y)

TEST_UPDATE_FUNCTION_LOGS_ID_AND_TIMESTAMP(DrawableGameComponentTest, DrawableGameComponent, 0, 0)

TEST(DrawableGameComponentTest, UsesTheConstructorPositionAsItsInitialPlacementAndSetsDirectionToRight) {
//    TODO: Could use parameterized tests instead of naive for loop, evaluate readability benefit
    const std::array<std::tuple<int, int>, 4> args = {
            std::make_tuple(0, 0),
            std::make_tuple(25, 25),
            std::make_tuple(25, 50),
            std::make_tuple(50, 25)
    };

    for (const auto [x, y]: args) {
        DrawableGameComponent gameComponent(x, y);

        ASSERT_EQ(gameComponent.getX(), x);
        ASSERT_EQ(gameComponent.getY(), y);
        ASSERT_EQ(gameComponent.getDirection(), DrawableGameComponent::Right);
    }
}

TEST(DrawableGameComponentTest, ChangesXAndYPositionBasedOnCurrentDirection) {
    DrawableGameComponent drawableGameComponent(4, 4);

    const auto timePoint = std::chrono::system_clock::now();

    drawableGameComponent.setDirection(DrawableGameComponent::Right);
    drawableGameComponent.update(timePoint);
    ASSERT_POS(drawableGameComponent, 5, 4);

    drawableGameComponent.setDirection(DrawableGameComponent::Left);
    drawableGameComponent.update(timePoint);
    ASSERT_POS(drawableGameComponent, 4, 4);

    drawableGameComponent.setDirection(DrawableGameComponent::Up);
    drawableGameComponent.update(timePoint);
    ASSERT_POS(drawableGameComponent, 4, 5);

    drawableGameComponent.setDirection(DrawableGameComponent::Down);
    drawableGameComponent.update(timePoint);
    ASSERT_POS(drawableGameComponent, 4, 4);
}

TEST(DrawableGameComponentTest, LogsTheNewPositionAndCurrentDirectionWhenUpdateIsCalled) {
    DrawableGameComponent drawableGameComponent(0, 0);

    testing::internal::CaptureStdout();
    drawableGameComponent.update(std::chrono::system_clock::now());
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::HasSubstr("Right: X:1 Y:0"));
}

TEST(DrawableGameComponentTest, ChangesDirectionWhenUpdateIsCalled) {
//    TODO: This test is not the cleanest, technically we should inject the random number provider dependency
//          so it can be mocked however to do so would be massively deviating from the contract defined in the task
//          assignment.
    DrawableGameComponent drawableGameComponent(0, 0);
    for (int i = 0; i < 10000; ++i) {
        auto previousDirection = drawableGameComponent.getDirection();
        drawableGameComponent.update(std::chrono::system_clock::now());
        ASSERT_NE(drawableGameComponent.getDirection(), previousDirection);
    }
}

TEST(DrawableGameComponentTest, ReturnsStringForProvidedDirectionWhenGetDirectionNameIsCalled) {
#define ASSERT_DIRECTION_STRING(DIRECTION, STRING) \
EXPECT_THAT(DrawableGameComponent::getDirectionName(DIRECTION), testing::Eq(STRING))

    ASSERT_DIRECTION_STRING(DrawableGameComponent::Right, "Right");
    ASSERT_DIRECTION_STRING(DrawableGameComponent::Left, "Left");
    ASSERT_DIRECTION_STRING(DrawableGameComponent::Up, "Up");
    ASSERT_DIRECTION_STRING(DrawableGameComponent::Down, "Down");
    ASSERT_DIRECTION_STRING(static_cast<DrawableGameComponent::Direction>(-1), "Unknown");
}

TEST(DrawableGameComponentTest, ShouldClampXAndYPositionWithinTheLegalBounds) {
#define CREATE_AND_UPDATE_COMPONENT(COMPONENT, INITIAL_X, INITIAL_Y, INITIAL_DIRECTION) \
    COMPONENT  = std::make_unique<DrawableGameComponent>(INITIAL_X, INITIAL_Y);         \
    COMPONENT->setDirection(INITIAL_DIRECTION);                                         \
    COMPONENT->update(std::chrono::system_clock::now());

    std::unique_ptr<DrawableGameComponent> drawableGameComponent;
    CREATE_AND_UPDATE_COMPONENT(drawableGameComponent, 0, 0, DrawableGameComponent::Left);
    ASSERT_POS((*drawableGameComponent), 0, 0);

    CREATE_AND_UPDATE_COMPONENT(drawableGameComponent, 0, 0, DrawableGameComponent::Down);
    ASSERT_POS((*drawableGameComponent), 0, 0);

    CREATE_AND_UPDATE_COMPONENT(drawableGameComponent, 79, 19, DrawableGameComponent::Right);
    ASSERT_POS((*drawableGameComponent), 79, 19);

    CREATE_AND_UPDATE_COMPONENT(drawableGameComponent, 79, 19, DrawableGameComponent::Up);
    ASSERT_POS((*drawableGameComponent), 79, 19);
}