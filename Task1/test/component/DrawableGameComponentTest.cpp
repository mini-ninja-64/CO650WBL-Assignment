#include <array>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "component/DrawableGameComponent.hpp"
#include "CommonTests.hpp"

#define ASSERT_POS(object, x, y) ASSERT_EQ(object.getX(), x); ASSERT_EQ(object.getY(), y)
#define ILLEGAL_DIRECTION static_cast<DrawableGameComponent::Direction>(-1)

TEST_UPDATE_FUNCTION_LOGS_ID_AND_TIMESTAMP(DrawableGameComponentTest, DrawableGameComponent, 0, 0)

TEST(DrawableGameComponentTest, UsesTheConstructorPositionAsItsInitialPlacementAndSetsDirectionToRight) {
    const std::array<std::tuple<int, int>, 4> args = {
            std::make_tuple(0, 0),
            std::make_tuple(25, 25),
            std::make_tuple(25, 50),
            std::make_tuple(50, 25)
    };

    for (const auto& [x, y]: args) {
        DrawableGameComponent gameComponent(x, y);

        ASSERT_EQ(gameComponent.getX(), x);
        ASSERT_EQ(gameComponent.getY(), y);
        ASSERT_EQ(gameComponent.getDirection(), DrawableGameComponent::Right);
    }
}

TEST(DrawableGameComponentTest, ChangesXAndYPositionBasedOnCurrentDirection) {
    DrawableGameComponent drawableGameComponent(4, 4);

    const auto timePoint = std::chrono::system_clock::now();
    const auto cTime = std::chrono::system_clock::to_time_t(timePoint);
    const auto* localTime = std::localtime(&cTime);


    drawableGameComponent.setDirection(DrawableGameComponent::Right);
    drawableGameComponent.update(localTime);
    ASSERT_POS(drawableGameComponent, 5, 4);

    drawableGameComponent.setDirection(DrawableGameComponent::Left);
    drawableGameComponent.update(localTime);
    ASSERT_POS(drawableGameComponent, 4, 4);

    drawableGameComponent.setDirection(DrawableGameComponent::Up);
    drawableGameComponent.update(localTime);
    ASSERT_POS(drawableGameComponent, 4, 5);

    drawableGameComponent.setDirection(DrawableGameComponent::Down);
    drawableGameComponent.update(localTime);
    ASSERT_POS(drawableGameComponent, 4, 4);
}

TEST(DrawableGameComponentTest, LogsTheNewPositionAndCurrentDirectionWhenUpdateIsCalled) {
    DrawableGameComponent drawableGameComponent(0, 0);

    const auto cTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const auto* localTime = std::localtime(&cTime);

    testing::internal::CaptureStdout();
    drawableGameComponent.update(localTime);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, testing::HasSubstr("Right: X:1 Y:0"));
}

TEST(DrawableGameComponentTest, ChangesDirectionWhenUpdateIsCalled) {
//    NOTE: This test is not the cleanest, in my opinion we should inject the random number provider dependency,
//          so it can be mocked however to do so would be deviating from the contract defined in the task
//          assignment.
    DrawableGameComponent drawableGameComponent(0, 0);
    for (int i = 0; i < 10000; ++i) {
        auto previousDirection = drawableGameComponent.getDirection();

        const auto cTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const auto* localTime = std::localtime(&cTime);

        drawableGameComponent.update(localTime);
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
    ASSERT_DIRECTION_STRING(ILLEGAL_DIRECTION, "Unknown");
}


TEST(DrawableGameComponentTest, ShouldClampXAndYPositionWithinTheLegalBounds) {
    const auto cTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    const auto* localTime = std::localtime(&cTime);

#define CREATE_AND_UPDATE_COMPONENT(COMPONENT, INITIAL_X, INITIAL_Y, INITIAL_DIRECTION) \
    COMPONENT  = std::make_unique<DrawableGameComponent>(INITIAL_X, INITIAL_Y);         \
    COMPONENT->setDirection(INITIAL_DIRECTION);                                         \
    COMPONENT->update(localTime);

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

TEST(DrawableGameComponentTest, ShouldThrowExceptionWhenSetDirectionIsCalledWithAnIllegalDirection) {
    ASSERT_THROW({
        DrawableGameComponent gameComponent(0,0);
        gameComponent.setDirection(ILLEGAL_DIRECTION);
    }, std::invalid_argument);
}