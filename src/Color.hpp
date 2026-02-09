#pragma once

#include <cstdint>

namespace TUIE {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

constexpr Color RED = {255, 0, 0};
constexpr Color GREEN = {0, 255, 0};
constexpr Color BLUE = {0, 0, 255};
constexpr Color YELLOW = {255, 255, 0};
constexpr Color MAGENTA = {255, 0, 255};
constexpr Color CYAN = {0, 255, 255};
constexpr Color WHITE = {255, 255, 255};
constexpr Color BLACK = {0, 0, 0};

}  // namespace TUIE