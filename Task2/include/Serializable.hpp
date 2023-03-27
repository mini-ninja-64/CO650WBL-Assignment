#pragma once

class Serializable {
public:
    [[maybe_unused]] [[nodiscard]] virtual std::vector<uint8_t> getBytes() = 0;
};
