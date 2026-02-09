#pragma once

#include <cstdint>
#include <ostream>

namespace TUIE {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    bool operator==(const Color& other) const { return r == other.r && g == other.g && b == other.b; }

    bool operator!=(const Color& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << "(" << static_cast<int>(color.r) << ", " << static_cast<int>(color.g) << ", " << static_cast<int>(color.b)
           << ")";
        return os;
    }
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