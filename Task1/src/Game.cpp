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
//        TODO: Throw error?
        std::cerr << "A nullptr cannot be added to the Game instance" << std::endl;
        return;
    }
    if(componentCount >= maximumComponentCount) {
//        TODO: Throw error?
        std::cerr << "GameComponent cannot be added due to the Game instance being full" << std::endl;
        return;
    }

    components[componentCount] = gameComponent;
    componentCount++;
}

void Game::run() {
    if(initialiseFunctionPointer != nullptr) initialiseFunctionPointer();

    const int ITERATIONS = 5;

    for (int iteration = 0; iteration < ITERATIONS; iteration++) {
        for (int componentIndex = 0; componentIndex < componentCount; componentIndex++) {
            components[componentIndex]->update(std::chrono::system_clock::now());
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
