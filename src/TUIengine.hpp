#pragma once

#include <chrono>

#include "Color.hpp"
#include "FixedOStream.hpp"
#include "Terminal.hpp"

namespace TUIE {

class engine {
   private:
    explicit engine();

   public:
    static engine &instance() {
        static engine instance;
        return instance;
    }

   public:
    TerminalSize get_terminal_size();
    bool window_should_close();
    void set_fps(int fps);
    void clear_background(Color color);
    void begin_draw();
    void end_draw();
    void draw_text(int x, int y, std::string_view text);
    void draw_rect(int x, int y, int width, int height, Color color);

   public:
    void on_resize();

   public:
    Terminal terminal;

   private:
    int fps = 30;
    std::chrono::high_resolution_clock::time_point last_frame_time;
};

}  // namespace TUIE