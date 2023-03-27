#pragma once

#include <cstdint>
#include <vector>

#include "Serializable.hpp"

class PacketBody: public Serializable {
public:
    virtual ~PacketBody() = default;

    [[maybe_unused]] [[nodiscard]] std::vector<uint8_t> getBytes() override = 0;
};
