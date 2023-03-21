#pragma once

#include <random>

template<typename T, T... providedStates>
T chooseRandomState() {
    const std::size_t validStatesCount = sizeof...(providedStates);

    std::random_device randomDevice;
    std::default_random_engine randomEngine(randomDevice());
    std::uniform_int_distribution<int> intDistribution(0, validStatesCount-1);

    const T validStates[] = {providedStates...};
    return validStates[intDistribution(randomEngine)];
}
