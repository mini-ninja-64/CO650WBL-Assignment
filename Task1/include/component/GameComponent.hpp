#pragma once

#include <chrono>

class GameComponent {
private:
    static int instances;
    int id;

public:
    GameComponent();
    GameComponent(const GameComponent&) = delete;
    virtual ~GameComponent() = default;

    /**
     * Update a game component, causing it to log out to stdout its id and the time at which it was updated.
     *
     * @param time a reference to the time at which the object is updated.
     */
    virtual void update(const tm* timePoint);

    [[nodiscard]] static int getInstances();
    [[nodiscard]] int getId() const;
};

