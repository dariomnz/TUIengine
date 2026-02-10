#pragma once

#include <termios.h>

#include "Color.hpp"

namespace TUIE {

struct TerminalSize {
    int width;
    int height;
};

class Terminal {
   public:
    Terminal();
    ~Terminal();

   public:
    void on_resize();
    TerminalSize get_terminal_size();

   private:
    void enable_raw_mode();
    void disable_raw_mode();
    void enter_fullscreen();
    void exit_fullscreen();

   public:
    void enable_line_wrapping(bool enable);
    void enable_cursor(bool enable);

    void clear_screen();
    void reset_cursor();
    void reset_colors();
    void reset_foreground();
    void reset_background();

    void set_cursor_position(int x, int y);
    void set_background_color(Color color);
    void set_foreground_color(Color color);

   public:
    TerminalSize size;

   private:
    termios original_termios;
};

}  // namespace TUIE