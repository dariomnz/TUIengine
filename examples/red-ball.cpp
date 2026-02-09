#include "Color.hpp"
#include "TUIengine.hpp"

int main() {
    TUIE::engine &engine = TUIE::engine::instance();
    int x = 0;
    int y = 0;
    int dx = 1;
    int dy = 1;
    while (!engine.window_should_close()) {
        int height = engine.terminal.size.height / 10;
        int width = height * 2;
        engine.begin_draw();
        engine.clear_background(TUIE::WHITE);
        engine.draw_text(0, 0, "Width: " + std::to_string(engine.terminal.size.width));
        engine.draw_text(0, 1, "Height: " + std::to_string(engine.terminal.size.height));
        engine.draw_rect(x, y, width, height, TUIE::RED);
        engine.end_draw();

        x += dx;
        y += dy;

        // Logic to permit the ball to bounce off the walls
        if (x > engine.terminal.size.width - width) {
            x = engine.terminal.size.width - width;
            dx = -1;
        }
        if (y > engine.terminal.size.height - height) {
            y = engine.terminal.size.height - height;
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