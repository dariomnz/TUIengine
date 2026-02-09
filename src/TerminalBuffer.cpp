#include "TerminalBuffer.hpp"

namespace TUIE {

TerminalBuffer::TerminalBuffer(int width, int height) : width(width), height(height), buffer(width * height) {}

void TerminalBuffer::resize(int new_width, int new_height) {
    // This takes into account that the resize expands the buffer to the left and the bottom without breaking the old
    // buffer data, it also takes into account that the new size can be smaller than the old size
    std::vector<TerminalCell> new_buffer(new_width * new_height);
    for (int i = 0; i < std::min(new_height, height); i++) {
        for (int j = 0; j < std::min(new_width, width); j++) {
            new_buffer[get_index(j, i, new_width)] = buffer[get_index(j, i, width)];
        }
    }
    buffer.swap(new_buffer);
    width = new_width;
    height = new_height;
}

TerminalCell TerminalBuffer::get_cell(int x, int y) const { return buffer[get_index(x, y)]; }

void TerminalBuffer::set_cell(int x, int y, TerminalCell cell) { buffer[get_index(x, y)] = cell; }

void TerminalBuffer::set_character(int x, int y, char character) { buffer[get_index(x, y)].character = character; }

void TerminalBuffer::set_foreground_color(int x, int y, Color color) {
    buffer[get_index(x, y)].foreground_color = color;
}

void TerminalBuffer::set_background_color(int x, int y, Color color) {
    buffer[get_index(x, y)].background_color = color;
}

inline int TerminalBuffer::get_index(int x, int y) const { return y * width + x; }

inline int TerminalBuffer::get_index(int x, int y, int width) const { return y * width + x; }

std::ostream& operator<<(std::ostream& os, const TerminalBuffer& buffer) {
    os << "Terminal buffer: Width: " << buffer.width << " Height: " << buffer.height << '\n';
    // Draw a border to the output buffer
    // First the top border
    os << "┌";
    for (int j = 0; j < buffer.width; j++) {
        os << "─";
    }
    os << "┐\n";
    // Then the side borders and the buffer content
    for (int i = 0; i < buffer.height; i++) {
        os << "│";
        for (int j = 0; j < buffer.width; j++) {
            os << buffer.get_cell(j, i).character;
        }
        os << "│";
        os << '\n';
    }
    // Then the bottom border
    os << "└";
    for (int j = 0; j < buffer.width; j++) {
        os << "─";
    }
    os << "┘\n";
    return os;
}
}  // namespace TUIE