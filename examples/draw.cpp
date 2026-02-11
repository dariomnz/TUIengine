#include "Color.hpp"
#include "TUIengine.hpp"
#include "Terminal.hpp"

int main() {
    TUIE::engine &engine = TUIE::engine::instance();
    engine.set_fps(60);
    while (!engine.window_should_close()) {
        engine.begin_draw();
        if (engine.get_input().is_mouse_pressed(TUIE::MOUSE_BUTTONS::LEFT)) {
            auto mouse_pos = engine.get_input().get_mouse_position();
            engine.draw_rect(mouse_pos.x, mouse_pos.y, 1, 1, TUIE::RED, 'O', TUIE::RED);
        }
        if (engine.get_input().is_mouse_pressed(TUIE::MOUSE_BUTTONS::RIGHT)) {
            auto mouse_pos = engine.get_input().get_mouse_position();
            engine.draw_rect(mouse_pos.x, mouse_pos.y, 1, 1, TUIE::TERMINAL_COLOR, ' ', TUIE::TERMINAL_COLOR);
        }
        engine.end_draw();
    }
}