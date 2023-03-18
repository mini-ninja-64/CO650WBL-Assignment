#include <random>
#include <iostream>
#include <array>

#include "component/DrawableGameComponent.hpp"
#include "util/random.hpp"

DrawableGameComponent::DrawableGameComponent(int x, int y) : x(x), y(y) {

}

void DrawableGameComponent::update(const std::chrono::time_point<std::chrono::system_clock> &timePoint) {
    GameComponent::update(timePoint);

    switch (direction) {
        case Right:
            x++;
            break;
        case Left:
            x--;
            break;
        case Up:
            y++;
            break;
        case Down:
            y--;
            break;
    }

    if(x < 0) x = 0;
    if(x >= SCREEN_WIDTH) x = SCREEN_WIDTH-1;
    if(y < 0) y = 0;
    if(y >= SCREEN_HEIGHT) y = SCREEN_HEIGHT-1;

    draw();
    changeDirection();
}

void DrawableGameComponent::draw() {
    std::cout << getDirectionName(direction) << ": X:" << x << " Y:" << y << std::endl;
}

//static const std::array<DrawableGameComponent::Direction, 4> allDirections = {
//        DrawableGameComponent::Left,
//        DrawableGameComponent::Right,
//        DrawableGameComponent::Up,
//        DrawableGameComponent::Down
//};

// TODO: Decide on implementation style
void DrawableGameComponent::changeDirection() {
//    State Machine Style
    switch (direction) {
        case Left:
            direction = chooseRandomState<Direction, Right, Up, Down>();
            break;
        case Right:
            direction = chooseRandomState<Direction, Left, Up, Down>();
            break;
        case Up:
            direction = chooseRandomState<Direction, Left, Right, Down>();
            break;
        case Down:
            direction = chooseRandomState<Direction, Left, Right, Up>();
            break;
    }

//    Random Choice Style
//    std::array<Direction, allDirections.size()-1> validDirections = {};
//    int index = 0;
//    for (auto possibleDirection: allDirections) {
//        if(direction == possibleDirection) continue;
//        validDirections[index] = possibleDirection;
//        index++;
//    }
//
//    std::random_device randomDevice;
//    std::default_random_engine randomEngine(randomDevice());
//    std::uniform_int_distribution<int> intDistribution(0, validDirections.size()-1);
//
//    const auto newDirectionIndex = intDistribution(randomEngine);
//    direction = validDirections[newDirectionIndex];
}

void DrawableGameComponent::setDirection(DrawableGameComponent::Direction newDirection) {
    DrawableGameComponent::direction = newDirection;
}

int DrawableGameComponent::getX() const {
    return x;
}

int DrawableGameComponent::getY() const {
    return y;
}

DrawableGameComponent::Direction DrawableGameComponent::getDirection() const {
    return direction;
}

std::string DrawableGameComponent::getDirectionName(DrawableGameComponent::Direction direction) {
    switch (direction) {
        case Right: return "Right";
        case Left: return "Left";
        case Up: return "Up";
        case Down: return "Down";
    }
    return "Unknown";
}

