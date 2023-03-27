#include "Game.hpp"

#include <iostream>
#include <thread>

Game::Game(int maximumComponentCount):
    maximumComponentCount(maximumComponentCount),
    components(new GameComponent*[maximumComponentCount] {nullptr}) {}

Game::~Game() {
    delete[] components;
}


void Game::add(GameComponent* gameComponent) {
    if(gameComponent == nullptr) {
        throw std::invalid_argument("A nullptr cannot be added to the Game instance");
    }
    if(componentCount >= maximumComponentCount) {
        throw std::length_error("The GameComponent cannot be added due to the Game instance being full");
    }

    components[componentCount] = gameComponent;
    componentCount++;
}

void Game::run() {
    if(initialiseFunctionPointer != nullptr) initialiseFunctionPointer();

    const int ITERATIONS = 5;

    for (int iteration = 0; iteration < ITERATIONS; iteration++) {
        for (int componentIndex = 0; componentIndex < componentCount; componentIndex++) {
            auto currentTime = std::chrono::system_clock::now();
            const auto cTime = std::chrono::system_clock::to_time_t(currentTime);
            const auto* localTime = std::localtime(&cTime);
            components[componentIndex]->update(localTime);
        }

        const std::chrono::milliseconds sleepDuration(TICK_1000MS);
        std::this_thread::sleep_for(sleepDuration);
    }

    if(terminateFunctionPointer != nullptr) terminateFunctionPointer();
}

void Game::setInitialise(InitialiseFunctionPointer pointer) {
    initialiseFunctionPointer = pointer;
}

void Game::setTerminate(TerminateFunctionPointer pointer) {
    terminateFunctionPointer = pointer;
}
