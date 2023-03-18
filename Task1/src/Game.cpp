#include "Game.hpp"

void Game::setInitialise(void (*functionPointer)()) {
    Game::initialise = functionPointer;
}

void Game::setTerminate(void (*functionPointer)()) {
    Game::terminate = functionPointer;
}

void Game::run() {
    if(initialise != nullptr) initialise();
    if(terminate != nullptr) terminate();
}

void Game::add(std::unique_ptr<GameComponent> gameComponent) {
//    gameComponents.push_back(gameComponent);
}
