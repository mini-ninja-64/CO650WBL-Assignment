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
//     TODO: the assignment brief says we should use TM
    virtual void update(const std::chrono::time_point<std::chrono::system_clock>& timePoint);

    [[nodiscard]] static int getInstances();
    [[nodiscard]] int getId() const;
};

