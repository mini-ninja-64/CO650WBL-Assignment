#pragma once

#include <chrono>

class GameComponent {
private:
//    TODO: make this atomic to prevent
    static int instances;
    int id;

public:
    GameComponent();

    /**
     * Update a game component, causing it to log out to stdout its id and the time at which it was updated.
     *
     * @param time a pointer to the time at which the object is updated???. TODO: this requirement is unclear
     */
    virtual void update(const std::chrono::time_point<std::chrono::system_clock>& timePoint);

    [[nodiscard]] static int getInstances();
    [[nodiscard]] int getId() const;
};

