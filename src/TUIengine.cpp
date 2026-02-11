

#include "TUIengine.hpp"

#include <sys/ioctl.h>
#include <unistd.h>

#include <chrono>
#include <csignal>
#include <thread>

#include "Terminal.hpp"
#include "TerminalBuffer.hpp"
#include "debug.hpp"

namespace TUIE {

engine::engine()
    : m_terminal(),
      m_buffer{TerminalBuffer(m_terminal.size.width, m_terminal.size.height),
               TerminalBuffer(m_terminal.size.width, m_terminal.size.height)} {
    std::signal(SIGINT, exit);
}

TerminalSize engine::get_terminal_size() { return m_terminal.size; }

void engine::on_resize() { m_resize_flag = true; }

bool engine::window_should_close() { return m_input.is_key_pressed(KEYS::ESCAPE) || m_input.is_key_pressed('q'); }

void engine::set_fps(int fps) { this->m_fps = fps; }

void engine::clear_background(Color color) { draw_rect(0, 0, m_terminal.size.width, m_terminal.size.height, color); }

void engine::begin_draw() {
    debug_msg("Begin draw");
    m_start_frame_time = std::chrono::high_resolution_clock::now();
    if (m_resize_flag) {
        m_terminal.on_resize();
        m_buffer[m_current_buffer].resize(m_terminal.size.width, m_terminal.size.height);
        m_resize_flag = false;
    }
    m_input.clear_events();
    m_input.process_input();
}

void engine::end_draw() {
    draw_buffer();
    m_current_buffer = next_buffer_index();
    fixedCout.flush();
    const auto target_time = std::chrono::microseconds(1000000 / m_fps);
    const auto sleep_time = target_time - (std::chrono::high_resolution_clock::now() - m_start_frame_time);
    m_real_fps =
        1000.0f / (std::chrono::duration_cast<std::chrono::microseconds>(target_time - sleep_time).count() / 1000.0f);
    debug_msg("End draw sleep for "
              << std::chrono::duration_cast<std::chrono::microseconds>(sleep_time).count() / 1000.0 << "ms used "
              << std::chrono::duration_cast<std::chrono::microseconds>(target_time - sleep_time).count() / 1000.0
              << "ms");
    if (sleep_time.count() > 0) {
        std::this_thread::sleep_for(sleep_time);
    }
}

void engine::draw_text(int x, int y, std::string_view text) {
    TerminalBuffer& current_buffer = get_current_buffer();
    for (int i = 0; i < text.size(); i++) {
        if (!current_buffer.is_inside(x + i, y)) {
            break;
        }
        current_buffer.set_character(x + i, y, text[i]);
    }
}

void engine::draw_text(int x, int y, std::string_view text, Color foreground_color) {
    TerminalBuffer& current_buffer = get_current_buffer();
    for (int i = 0; i < text.size(); i++) {
        if (!current_buffer.is_inside(x + i, y)) {
            break;
        }
        current_buffer.set_character(x + i, y, text[i]);
        current_buffer.set_foreground_color(x + i, y, foreground_color);
    }
}

void engine::draw_text(int x, int y, std::string_view text, Color foreground_color, Color background_color) {
    TerminalBuffer& current_buffer = get_current_buffer();
    for (int i = 0; i < text.size(); i++) {
        if (!current_buffer.is_inside(x + i, y)) {
            break;
        }
        current_buffer.set_cell(x + i, y, TerminalCell{text[i], foreground_color, background_color});
    }
}

void engine::draw_rect(int x, int y, int width, int height, Color color, char character, Color character_color) {
    TerminalBuffer& current_buffer = get_current_buffer();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (!current_buffer.is_inside(x + j, y + i)) {
                break;
            }
            current_buffer.set_cell(x + j, y + i, TerminalCell{character, character_color, color});
        }
    }
}

TerminalBuffer& engine::get_current_buffer() { return m_buffer[m_current_buffer]; }
TerminalBuffer& engine::get_back_buffer() { return m_buffer[next_buffer_index()]; }
int engine::next_buffer_index() { return (m_current_buffer + 1) % 2; }

void engine::draw_buffer() {
    // This function compare the current buffer with the previous buffer and only prints the changes
    TerminalBuffer& current_buffer = get_current_buffer();
    TerminalBuffer& previous_buffer = get_back_buffer();

    bool first_bg = false, first_fg = false;
    Color last_bg = {0, 0, 0};
    Color last_fg = {0, 0, 0};
    bool cursor_moved = true;

    debug_msg("Drawing previous buffer\n" << previous_buffer);
    debug_msg("Drawing buffer\n" << current_buffer);
    m_terminal.reset_cursor();
    m_terminal.reset_colors();
    for (int y = 0; y < current_buffer.get_height(); y++) {
        for (int x = 0; x < current_buffer.get_width(); x++) {
            const TerminalCell current_cell = current_buffer.get_cell(x, y);
            bool cells_equal = false;
            if (previous_buffer.is_inside(x, y)) {
                cells_equal = current_cell == previous_buffer.get_cell(x, y);
            }

            if (!cells_equal) {
                if (cursor_moved) {
                    m_terminal.set_cursor_position(x + 1, y + 1);
                    cursor_moved = false;
                    debug_msg("Cursor moved to " << x << ", " << y + 1);
                }
                if (current_cell.background_color != last_bg || !first_bg) {
                    m_terminal.set_background_color(current_cell.background_color);
                    last_bg = current_cell.background_color;
                    first_bg = true;
                    debug_msg("Background color changed to " << current_cell.background_color);
                }
                if (current_cell.foreground_color != last_fg || !first_fg) {
                    m_terminal.set_foreground_color(current_cell.foreground_color);
                    last_fg = current_cell.foreground_color;
                    first_fg = true;
                    debug_msg("Foreground color changed to " << current_cell.foreground_color);
                }
                fixedCout << current_cell.character;
                debug_msg("Character printed '" << current_cell.character << "' at " << x << ", " << y);
            } else {
                cursor_moved = true;
            }
        }
        cursor_moved = true;
    }
    previous_buffer = current_buffer;
}

}  // namespace TUIE
