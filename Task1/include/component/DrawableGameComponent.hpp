#pragma once

#include <chrono>
#include <string>
#include <random>

#include "GameComponent.hpp"

class DrawableGameComponent : public GameComponent {
public:
    DrawableGameComponent(int x, int y);
    DrawableGameComponent(const DrawableGameComponent&) = delete;

#define NUMBER_OF_DIRECTIONS 4
    enum Direction {
        Left,
        Right,
        Up,
        Down
    };

    /**
     * Update a game component, causing it to log out to stdout its id and the time at which it was updated.
     *
     * Then carrying out the DrawableGameComponent update process:
     * 1. Move the current position +/-1 based on the current direction
     * 2. Trigger a draw call
     * 3. Trigger a direction change
     *
     * @param time The time the object is updated.
     */
    void update(const std::chrono::time_point<std::chrono::system_clock> & timePoint) override;

    void setDirection(Direction newDirection);

    [[nodiscard]] int getX() const;
    [[nodiscard]] int getY() const;
    [[nodiscard]] Direction getDirection() const;

    /**
     * Get the String representation of the provided direction.
     *
     * @param direction The direction to convert to a string.
     */
    [[nodiscard]] static std::string getDirectionName(Direction direction);
private:
    const int SCREEN_WIDTH = 80;
    const int SCREEN_HEIGHT = 20;

//    TODO: Should be atomic for thread safety
    int x;
    int y;

//    TODO: Should be atomic for thread safety
    Direction direction = Right;

    std::default_random_engine randomEngine;
    std::uniform_int_distribution<int>  randomDistribution;

    /**
     * Assign a random direction to the direction data member which differs from the objects current direction
     */
    void changeDirection();

    /**
     * Display the current direction along with x and y value by writing to stdout.
     */
    void draw();
};
