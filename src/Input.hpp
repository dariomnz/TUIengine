#pragma once

#include <array>
#include <cstring>
#include <ostream>
#include <vector>

namespace TUIE {

enum class KEYS {
    NONE,
    CHARACTER,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ESC,
    SPACE,
    ENTER,
    TAB,
    ESCAPE,
    BACKSPACE,
    DELETE,
    INSERT,
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
};
std::ostream &operator<<(std::ostream &os, const KEYS &key);

enum class MOUSE_BUTTONS {
    NONE,
    LEFT,
    RIGHT,
    MIDDLE,
    WHEEL_UP,
    WHEEL_DOWN,
    POSITION,
    DRAG,
    SIZE,
};
std::ostream &operator<<(std::ostream &os, const MOUSE_BUTTONS &button);

enum class MOUSE_ACTION {
    NONE,
    PRESSED,
    RELEASED,
};
std::ostream &operator<<(std::ostream &os, const MOUSE_ACTION &action);

struct KeyboardEvent {
    KEYS key;
    char character;

    friend std::ostream &operator<<(std::ostream &os, const KeyboardEvent &event);
};

struct MousePosition {
    int x;
    int y;
};

struct MouseEvent {
    MOUSE_BUTTONS button;
    MousePosition position;
    MOUSE_ACTION action;

    friend std::ostream &operator<<(std::ostream &os, const MouseEvent &event);
};

struct PasteEvent {
    const char *text;
    size_t size;

    friend std::ostream &operator<<(std::ostream &os, const PasteEvent &event);
};

struct InputEvent {
    enum class type_t {
        Keyboard,
        Mouse,
        Paste,
    };
    type_t type;
    union {
        KeyboardEvent keyboardEvent;
        MouseEvent mouseEvent;
        PasteEvent pasteEvent;
    } as;

    InputEvent(KEYS key) : type(type_t::Keyboard), as({.keyboardEvent = {key, '\0'}}) {}
    InputEvent(char c) : type(type_t::Keyboard), as({.keyboardEvent = {KEYS::CHARACTER, c}}) {}
    InputEvent(MouseEvent mouseEvent) : type(type_t::Mouse), as({.mouseEvent = mouseEvent}) {}
    InputEvent(std::string_view sv)
        : type(type_t::Paste), as({.pasteEvent = {.text = ::strndup(sv.data(), sv.size()), .size = sv.size()}}) {}

    ~InputEvent() {
        if (type == type_t::Paste) {
            if (as.pasteEvent.text != nullptr) {
                std::free(const_cast<char *>(as.pasteEvent.text));
            }
        }
    }
    // Copy and move contructors to take into account the Paste event pointer
    InputEvent(const InputEvent &other) {
        type = other.type;
        switch (type) {
            case type_t::Keyboard:
                as.keyboardEvent = other.as.keyboardEvent;
                break;
            case type_t::Mouse:
                as.mouseEvent = other.as.mouseEvent;
                break;
            case type_t::Paste:
                as.pasteEvent = other.as.pasteEvent;
                as.pasteEvent.text = ::strndup(other.as.pasteEvent.text, other.as.pasteEvent.size);
                break;
        }
    }
    InputEvent(InputEvent &&other) {
        type = other.type;
        switch (type) {
            case type_t::Keyboard:
                as.keyboardEvent = other.as.keyboardEvent;
                break;
            case type_t::Mouse:
                as.mouseEvent = other.as.mouseEvent;
                break;
            case type_t::Paste:
                as.pasteEvent = other.as.pasteEvent;
                other.as.pasteEvent.text = nullptr;
                other.as.pasteEvent.size = 0;
                break;
        }
    }
    friend std::ostream &operator<<(std::ostream &os, const InputEvent &event);
};

class Input {
   public:
    void process_input();
    std::vector<InputEvent> &get_events() { return m_events; }
    void clear_events() { m_events.clear(); }

   public:
    bool is_key_pressed(char c);
    bool is_key_pressed(KEYS key);
    bool is_key_released(KEYS key);
    bool is_mouse_pressed(MOUSE_BUTTONS button);
    bool is_mouse_released(MOUSE_BUTTONS button);
    MousePosition get_mouse_position();
    bool is_scroll_down();
    bool is_scroll_up();

   private:
    int get_byte();
    bool have_to_read();
    void add_event(InputEvent event) { m_events.push_back(event); }
    void process_mouse_input();
    void handle_key(char byte);

   private:
    MousePosition m_mouse_position;
    std::array<MOUSE_ACTION, static_cast<int>(MOUSE_BUTTONS::SIZE)> m_mouse_state;
    std::vector<InputEvent> m_events;

    enum class State { NORMAL, ESC, CSI, MOUSE, PASTE_START, PASTE_CONTENT, PASTE_END };

    State m_state = State::NORMAL;
    std::string m_buffer;
};

}  // namespace TUIE