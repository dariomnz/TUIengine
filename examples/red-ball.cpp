#include "Color.hpp"
#include "TUIengine.hpp"
#include "Terminal.hpp"

int main() {
    TUIE::engine &engine = TUIE::engine::instance();
    int x = 0;
    int y = 0;
    int dx = 1;
    int dy = 1;
    engine.set_fps(60);
    while (!engine.window_should_close()) {
        TUIE::TerminalSize size = engine.get_terminal_size();
        int height = size.height / 10;
        int width = height * 2;
        engine.begin_draw();
        engine.clear_background(TUIE::WHITE);
        engine.draw_rect(x, y, width, height, TUIE::RED);
        engine.draw_text(
            0, 0,
            "FPS: " + std::to_string(engine.get_target_fps()) + " Real FPS: " + std::to_string(engine.get_real_fps()),
            TUIE::BLACK);
        engine.draw_text(0, 1, "Width: " + std::to_string(size.width), TUIE::BLACK);
        engine.draw_text(0, 2, "Height: " + std::to_string(size.height), TUIE::BLACK);
        engine.end_draw();

        x += dx;
        y += dy;

        // Logic to permit the ball to bounce off the walls
        if (x > size.width - width) {
            x = size.width - width;
            dx = -1;
        }
        if (y > size.height - height) {
            y = size.height - height;
            dy = -1;
        }
        if (x < 0) {
            x = 0;
            dx = 1;
        }
        if (y < 0) {
            y = 0;
            dy = 1;
        }
    }
}