#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Color.hpp"
#include "Input.hpp"
#include "TUIengine.hpp"
#include "Terminal.hpp"

struct WrappedLine {
    std::string text;
    int original_index;
};

std::vector<WrappedLine> wrap_content(const std::vector<std::string>& raw_lines, int width) {
    std::vector<WrappedLine> wrapped;
    if (width < 1) return wrapped;

    for (int i = 0; i < (int)raw_lines.size(); ++i) {
        const auto& line = raw_lines[i];
        if (line.empty()) {
            wrapped.push_back({"", i + 1});
            continue;
        }

        for (size_t k = 0; k < line.length(); k += width) {
            wrapped.push_back({line.substr(k, width), i + 1});
        }
    }
    return wrapped;
}

int main(int argc, char* argv[]) {
    // Check if filename is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    // Read file content
    std::vector<std::string> lines;
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    // Initialize engine
    TUIE::engine& engine = TUIE::engine::instance();
    engine.set_fps(60);

    int scroll_offset = 0;
    std::vector<WrappedLine> display_lines;
    int last_terminal_width = 0;

    while (!engine.window_should_close()) {
        engine.begin_draw();
        engine.clear_background(TUIE::TERMINAL_COLOR);

        TUIE::TerminalSize size = engine.get_terminal_size();
        int content_height = size.height - 1;
        if (content_height < 1) content_height = 1;

        // Recalculate wrapping if width changed
        if (size.width != last_terminal_width) {
            display_lines = wrap_content(lines, size.width);
            last_terminal_width = size.width;

            // Adjust scroll if out of bounds
            if (scroll_offset >= (int)display_lines.size()) {
                scroll_offset = std::max(0, (int)display_lines.size() - content_height);
            }
        }

        int total_display_lines = (int)display_lines.size();
        int max_scroll = std::max(0, total_display_lines - content_height);

        // Input Handling
        // Down
        if (engine.get_input().is_key_pressed(TUIE::KEYS::DOWN) || engine.get_input().is_key_pressed('j') ||
            engine.get_input().is_scroll_down()) {
            if (scroll_offset < max_scroll) {
                scroll_offset++;
            }
        }
        // Up
        if (engine.get_input().is_key_pressed(TUIE::KEYS::UP) || engine.get_input().is_key_pressed('k') ||
            engine.get_input().is_scroll_up()) {
            if (scroll_offset > 0) {
                scroll_offset--;
            }
        }
        // Page Down
        if (engine.get_input().is_key_pressed(TUIE::KEYS::PAGE_DOWN) ||
            engine.get_input().is_key_pressed(TUIE::KEYS::SPACE)) {
            if (scroll_offset < max_scroll) {
                scroll_offset += content_height;
                if (scroll_offset > max_scroll) scroll_offset = max_scroll;
            }
        }
        // Page Up
        if (engine.get_input().is_key_pressed(TUIE::KEYS::PAGE_UP) || engine.get_input().is_key_pressed('b')) {
            scroll_offset -= content_height;
            if (scroll_offset < 0) {
                scroll_offset = 0;
            }
        }
        // Home
        if (engine.get_input().is_key_pressed(TUIE::KEYS::HOME) || engine.get_input().is_key_pressed('g')) {
            scroll_offset = 0;
        }
        // End
        if (engine.get_input().is_key_pressed(TUIE::KEYS::END) || engine.get_input().is_key_pressed('G')) {
            scroll_offset = max_scroll;
        }

        // Render content
        for (int i = 0; i < content_height; ++i) {
            int line_index = scroll_offset + i;
            if (line_index >= 0 && line_index < total_display_lines) {
                engine.draw_text(0, i, display_lines[line_index].text, TUIE::WHITE);
            }
        }

        // Render Status Bar
        std::ostringstream ss;
        ss << " " << argv[1] << " ";
        if (lines.empty()) {
            ss << "(Empty file) ";
        } else {
            int start_line = display_lines[scroll_offset].original_index;
            int end_idx = std::min(scroll_offset + content_height - 1, total_display_lines - 1);
            int end_line = display_lines[end_idx].original_index;
            ss << "Lines " << (start_line) << "-" << (end_line) << "/" << (lines.size());
        }
        ss << " (Press q to quit)";

        // Draw white bar at bottom
        engine.draw_rect(0, size.height - 1, size.width, 1, TUIE::WHITE, ' ', TUIE::BLACK);
        // Draw black text on white bar
        engine.draw_text(0, size.height - 1, ss.str(), TUIE::BLACK);

        engine.end_draw();
    }

    return 0;
}