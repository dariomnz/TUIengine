#pragma once

#include <chrono>

#include "Color.hpp"
#include "FixedOStream.hpp"
#include "Terminal.hpp"
#include "TerminalBuffer.hpp"

namespace TUIE {

class engine {
   private:
    explicit engine();

   public:
    static engine& instance() {
        static engine instance;
        return instance;
    }

   public:
    TerminalSize get_terminal_size();
    bool window_should_close();
    void set_fps(int fps);
    int get_target_fps() const { return fps; }
    float get_real_fps() const { return real_fps; }
    void clear_background(Color color);
    void begin_draw();
    void end_draw();
    void draw_text(int x, int y, std::string_view text);
    void draw_text(int x, int y, std::string_view text, Color foreground_color);
    void draw_text(int x, int y, std::string_view text, Color foreground_color, Color background_color);
    void draw_rect(int x, int y, int width, int height, Color color);

   public:
    void on_resize();

   private:
    void draw_buffer();
    TerminalBuffer& get_current_buffer();
    TerminalBuffer& get_back_buffer();
    int next_buffer_index();

   private:
    Terminal terminal;
    int fps = 30;
    float real_fps = 30.0f;
    std::chrono::high_resolution_clock::time_point start_frame_time;
    bool resize_flag = false;
    TerminalBuffer buffer[2];
    int current_buffer = 0;
};

}  // namespace TUIE