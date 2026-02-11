#include "Terminal.hpp"

#include <sys/ioctl.h>

#include <csignal>

#include "FixedOStream.hpp"
#include "TUIengine.hpp"

namespace TUIE {
void handle_sigwinch(int) { engine::instance().on_resize(); }

Terminal::Terminal() : size(get_terminal_size()) {
    std::signal(SIGWINCH, handle_sigwinch);
    enable_raw_mode();
    enable_cursor(false);
    enable_mouse(true);
    enable_mouse_move(true);
    enable_bracketed_paste(true);
    enable_line_wrapping(false);
    enter_fullscreen();
    fixedCout.flush();
}

Terminal::~Terminal() {
    std::signal(SIGWINCH, SIG_DFL);
    exit_fullscreen();
    enable_line_wrapping(true);
    enable_cursor(true);
    enable_mouse(false);
    enable_mouse_move(false);
    enable_bracketed_paste(false);
    disable_raw_mode();
    reset_cursor();
    reset_colors();
    clear_screen();
    fixedCout.flush();
}

void Terminal::enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    struct termios raw = original_termios;

    // Disable ECHO: don't show what you type
    // Disable ICANON: read byte by byte, no wait for 'Enter'
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);

    // Disable flow control (Ctrl+S, Ctrl+Q) and translation of CR to NL
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

    // Set timeout for the read (so it doesn't block the engine)
    raw.c_cc[VMIN] = 0;   // Read 0 or more bytes
    raw.c_cc[VTIME] = 1;  // Wait maximum 100ms (0.1s)

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void Terminal::disable_raw_mode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios); }

void Terminal::on_resize() { size = get_terminal_size(); }

TerminalSize Terminal::get_terminal_size() {
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {w.ws_col, w.ws_row};
}

void Terminal::enter_fullscreen() { fixedCout << "\033[?1049h"; }
void Terminal::exit_fullscreen() { fixedCout << "\033[?1049l"; }
void Terminal::enable_line_wrapping(bool enable) { fixedCout << (enable ? "\033[?7h" : "\033[?7l"); }
void Terminal::enable_cursor(bool enable) { fixedCout << (enable ? "\033[?25h" : "\033[?25l"); }
void Terminal::enable_mouse(bool enable) {
    fixedCout << (enable ? "\033[?1000h\033[?1006h" : "\033[?1000l\033[?1006l");
}
void Terminal::enable_bracketed_paste(bool enable) { fixedCout << (enable ? "\033[?2004h" : "\033[?2004l"); }
void Terminal::enable_mouse_move(bool enable) { fixedCout << (enable ? "\033[?1003h" : "\033[?1003l"); }

void Terminal::clear_screen() { fixedCout << "\033[2J"; }
void Terminal::reset_cursor() { fixedCout << "\033[H"; }
void Terminal::reset_colors() { fixedCout << "\033[0m"; }
void Terminal::reset_foreground() { fixedCout << "\033[39m"; }
void Terminal::reset_background() { fixedCout << "\033[49m"; }
void Terminal::set_cursor_position(int x, int y) { fixedCout << "\033[" << y << ";" << x << "H"; }
void Terminal::set_background_color(Color color) {
    if (color.without_color) {
        reset_background();
    } else {
        fixedCout << "\033[48;2;" << (int)color.r << ";" << (int)color.g << ";" << (int)color.b << "m";
    }
}
void Terminal::set_foreground_color(Color color) {
    if (color.without_color) {
        reset_foreground();
    } else {
        fixedCout << "\033[38;2;" << (int)color.r << ";" << (int)color.g << ";" << (int)color.b << "m";
    }
}

}  // namespace TUIE