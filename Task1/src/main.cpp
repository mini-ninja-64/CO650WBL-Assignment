#include <iostream>

#include "Game.hpp"
#include "component/GameComponent.hpp"
#include "component/DrawableGameComponent.hpp"

static void initialiseFunction() {
    std::cout << "Initialising game" << std::endl;
}

static void terminateFunction() {
    std::cout << "Terminating game" << std::endl;
}

#ifndef TESTING
int main() {
    Game* const game = new Game(5);

    game->setInitialise(initialiseFunction);
    game->setTerminate(terminateFunction);

    GameComponent gameComponent;
    game->add(&gameComponent);

    DrawableGameComponent drawableGameComponent(0,0);
    game->add(&drawableGameComponent);

    game->run();

    delete game;
}
#endif