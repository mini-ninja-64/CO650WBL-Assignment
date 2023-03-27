#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "component/GameComponent.hpp"
#include "Game.hpp"

auto generateTimePointCaptureFunction(std::vector<std::chrono::time_point<std::chrono::system_clock>>& vector) {
    return [vector](const tm* localTimePointer) mutable {
        tm localTimeCopy = *localTimePointer;
        const auto cTime = std::mktime(&localTimeCopy);

        vector.push_back(std::chrono::system_clock::from_time_t(cTime));
    };
}

template<typename T>
auto assertPairDistances(auto expectedDistance, auto tolerance, const std::vector<T>& vector) {
    if(vector.empty()) return;

    for (size_t i = 0; i < vector.size()-1; i++) {
        auto start = vector[i];
        auto end = vector[i+1];
        auto actualDistance = end-start;

        EXPECT_THAT(expectedDistance,testing::AllOf(
                testing::Ge(actualDistance-tolerance),
                testing::Le(actualDistance+tolerance)
        ));
    }
}

class MockGameComponent : public GameComponent {
public:
    MOCK_METHOD(void, update, (const tm* timePoint), (override));
};

TEST(GameTest, CallsUpdateOnEachProvidedGameComponent5TimesWithADelayOf1Second) {
    MockGameComponent gameComponent1, gameComponent2, gameComponent3;

    Game game(5);
    game.add(&gameComponent1);
    game.add(&gameComponent2);
    game.add(&gameComponent3);

    std::vector<std::chrono::system_clock::time_point> timePoints1, timePoints2, timePoints3;

    EXPECT_CALL(gameComponent1, update(testing::_))
        .Times(5)
        .WillRepeatedly(generateTimePointCaptureFunction(timePoints1));

    EXPECT_CALL(gameComponent2, update(testing::_))
            .Times(5)
            .WillRepeatedly(generateTimePointCaptureFunction(timePoints2));

    EXPECT_CALL(gameComponent3, update(testing::_))
            .Times(5)
            .WillRepeatedly(generateTimePointCaptureFunction(timePoints3));

    game.run();

    const std::chrono::milliseconds expectedDelay(1000);
    const std::chrono::milliseconds tolerance(500);

    assertPairDistances(expectedDelay, tolerance, timePoints1);
    assertPairDistances(expectedDelay, tolerance, timePoints2);
    assertPairDistances(expectedDelay, tolerance, timePoints3);
}

TEST(GameTest, ShouldThrowErrorWhenMaxComponentsHasBeenReached) {
    Game game(1);

    MockGameComponent mockGameComponent1, mockGameComponent2;

    game.add(&mockGameComponent1);
    EXPECT_THROW({
        game.add(&mockGameComponent2);
    }, std::length_error);
}

TEST(GameTest, ShouldThrowErrorWhenANullpointerIsAdded) {
    Game game(5);
    EXPECT_THROW({
        game.add(nullptr);
    }, std::invalid_argument);

}

static int mockInvocations = 0;
static void mockFunction() {
    mockInvocations++;
}

TEST(GameTest, CallsInitialiseFunctionPointerWhenRunIsCalled) {
    Game game(5);

    game.setInitialise(mockFunction);

    int startValue = mockInvocations;

    game.run();
    EXPECT_EQ(mockInvocations, startValue+1);
}

TEST(GameTest, CallsTerminateFunctionPointerWhenRunIsCalled) {
    Game game(5);

    game.setTerminate(mockFunction);

    int startValue = mockInvocations;

    game.run();
    EXPECT_EQ(mockInvocations, startValue+1);
}

TEST(GameTest, DoesNotThrowAnExceptionWhenFunctionPointersAreNull) {
    Game game(5);

    game.setInitialise(nullptr);
    game.setTerminate(nullptr);

    EXPECT_NO_THROW({
        game.run();
    });
}
