#pragma once

#include <vector>

#include "Color.hpp"

namespace TUIE {

struct TerminalCell {
    char character = ' ';
    Color foreground_color = WHITE;
    Color background_color = BLACK;

    bool operator==(const TerminalCell& other) const {
        return character == other.character && foreground_color == other.foreground_color &&
               background_color == other.background_color;
    }

    bool operator!=(const TerminalCell& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const TerminalCell& cell) {
        os << "['" << cell.character << "', " << cell.foreground_color << ", " << cell.background_color << "]";
        return os;
    }
};

class TerminalBuffer {
   public:
    TerminalBuffer(int width, int height);

    void resize(int width, int height);
    TerminalCell get_cell(int x, int y) const;
    void set_cell(int x, int y, TerminalCell cell);
    void set_character(int x, int y, char character);
    void set_foreground_color(int x, int y, Color color);
    void set_background_color(int x, int y, Color color);

   private:
    inline int get_index(int x, int y) const;
    inline int get_index(int x, int y, int width) const;

   public:
    int get_width() const { return width; }
    int get_height() const { return height; }

   private:
    std::vector<TerminalCell> buffer;
    int width;
    int height;

    auto begin() { return buffer.begin(); }
    auto end() { return buffer.end(); }

    friend std::ostream& operator<<(std::ostream& os, const TerminalBuffer& buffer);
};

}  // namespace TUIE