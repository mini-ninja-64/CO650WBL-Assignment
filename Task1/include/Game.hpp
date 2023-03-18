#pragma once

#include <memory>
#include <vector>

#include "component/GameComponent.hpp"

class Game {
public:
    Game(size_t maxSize);

    void run();
    void add(std::unique_ptr<GameComponent> gameComponent);

    void setInitialise(void (*functionPointer)());
    void setTerminate(void (*functionPointer)());

private:
//    std::vector<std::unique_ptr<GameComponent>> gameComponents;
    size_t componentCount = 0;

    void(*initialise)() = nullptr;
    void(*terminate)() = nullptr;
};
