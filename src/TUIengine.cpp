

#include "TUIengine.hpp"

#include <sys/ioctl.h>
#include <unistd.h>

#include <csignal>
#include <thread>

namespace TUIE {

engine::engine() { std::signal(SIGINT, exit); }

void engine::on_resize() { terminal.on_resize(); }

bool engine::window_should_close() { return false; }

void engine::clear_background(Color color) {
    terminal.set_cursor_position(0, 0);
    terminal.set_background_color(color);
    for (int i = 0; i < terminal.size.height; i++) {
        for (int j = 0; j < terminal.size.width; j++) {
            fixedCout << " ";
        }
        terminal.set_cursor_position(0, i + 1);
    }
    terminal.reset_colors();
}

void engine::begin_draw() {
    terminal.clear_screen();
    terminal.reset_cursor();
    terminal.reset_colors();
}

void engine::end_draw() {
    fixedCout.flush();
    std::this_thread::sleep_for(std::chrono::microseconds(
        1000000 / fps - (std::chrono::high_resolution_clock::now() - last_frame_time).count()));
    last_frame_time = std::chrono::high_resolution_clock::now();
}

void engine::draw_text(int x, int y, std::string_view text) {
    terminal.set_cursor_position(x, y + 1);
    fixedCout << text;
}

void engine::draw_rect(int x, int y, int width, int height, Color color) {
    terminal.set_cursor_position(x, y + 1);
    terminal.set_background_color(color);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fixedCout << " ";
        }
        terminal.set_cursor_position(x, y + i + 1);
    }
    terminal.reset_colors();
}

}  // namespace TUIE
