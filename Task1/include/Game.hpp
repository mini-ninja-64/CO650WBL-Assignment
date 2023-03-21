#pragma once

#include <memory>

#include "component/GameComponent.hpp"

typedef void (*VoidFunctionPointer)();
typedef VoidFunctionPointer InitialiseFunctionPointer;
typedef VoidFunctionPointer TerminateFunctionPointer;

class Game {
public:
    explicit Game(int maximumComponentCount);
    ~Game();

    void add(GameComponent* gameComponent);

    void run();

    void setInitialise(InitialiseFunctionPointer pointer);
    void setTerminate(TerminateFunctionPointer pointer);
private:
    const int TICK_1000MS = 1000;

    int maximumComponentCount;
    int componentCount = 0;
    GameComponent** components;

    InitialiseFunctionPointer initialiseFunctionPointer = nullptr;
    TerminateFunctionPointer terminateFunctionPointer = nullptr;

};