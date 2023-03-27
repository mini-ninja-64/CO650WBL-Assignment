#include <iostream>

#include "component/DrawableGameComponent.hpp"

DrawableGameComponent::DrawableGameComponent(int x, int y):
    x(x),
    y(y),
    randomEngine(
            std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()
            ).count()
    ),
    // The random distribution is configured for 1 less than necessary as we
    // never want to changeDirection to the current direction, as defined in the requirements
    // by doing this we can offset our random numbers to avoid collisions
    randomDirectionDistribution(0, NUMBER_OF_DIRECTIONS - 1 - 1) {}

void DrawableGameComponent::update(const tm* timePoint) {
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

void DrawableGameComponent::changeDirection() {
    int newDirectionIndex = randomDirectionDistribution(randomEngine);

    if (newDirectionIndex >= direction) newDirectionIndex++;

    direction = static_cast<Direction>(newDirectionIndex);
}

void DrawableGameComponent::setDirection(DrawableGameComponent::Direction newDirection) {
    if(newDirection != Right && newDirection != Left && newDirection != Up && newDirection != Down) {
        throw std::invalid_argument("Provided illegal direction");
    }
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

