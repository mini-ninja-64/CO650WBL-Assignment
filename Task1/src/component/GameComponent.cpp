#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

#include "component/GameComponent.hpp"

int GameComponent::instances = 0;

GameComponent::GameComponent(): id(++GameComponent::instances) {
}

void GameComponent::update(const tm* localTime) {
    std::cout << "ID: " << id << " Updated @ " << std::put_time(localTime, "%H:%M:%S") << std::endl;
}

int GameComponent::getInstances() {
    return instances;
}

int GameComponent::getId() const {
    return id;
}

