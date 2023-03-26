#pragma once

#include <cstdint>
#include <vector>

#include "Serializable.hpp"

class PacketHeader: Serializable {
public:
    virtual ~PacketHeader() = default;

    [[maybe_unused]] [[nodiscard]] std::vector<uint8_t> getBytes() override = 0;
};
