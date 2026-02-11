#include "Input.hpp"

#include <poll.h>
#include <unistd.h>

#include "debug.hpp"

namespace TUIE {

std::ostream &operator<<(std::ostream &os, const KEYS &key) {
#define CASE_KEY(key) \
    case KEYS::key:   \
        return os << #key;
    switch (key) {
        CASE_KEY(NONE);
        CASE_KEY(UP);
        CASE_KEY(DOWN);
        CASE_KEY(LEFT);
        CASE_KEY(RIGHT);
        CASE_KEY(ESC);
        CASE_KEY(SPACE);
        CASE_KEY(ENTER);
        CASE_KEY(TAB);
        CASE_KEY(ESCAPE);
        CASE_KEY(BACKSPACE);
        CASE_KEY(DELETE);
        CASE_KEY(INSERT);
        CASE_KEY(HOME);
        CASE_KEY(END);
        CASE_KEY(PAGE_UP);
        CASE_KEY(PAGE_DOWN);
        CASE_KEY(F1);
        CASE_KEY(F2);
        CASE_KEY(F3);
        CASE_KEY(F4);
        CASE_KEY(F5);
        CASE_KEY(F6);
        CASE_KEY(F7);
        CASE_KEY(F8);
        CASE_KEY(F9);
        CASE_KEY(F10);
        CASE_KEY(F11);
        CASE_KEY(F12);
        case KEYS::CHARACTER:
            return os;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const MOUSE_BUTTONS &button) {
#define CASE_MOUSE_BUTTONS(key) \
    case MOUSE_BUTTONS::key:    \
        return os << #key;
    switch (button) {
        CASE_MOUSE_BUTTONS(NONE);
        CASE_MOUSE_BUTTONS(LEFT);
        CASE_MOUSE_BUTTONS(RIGHT);
        CASE_MOUSE_BUTTONS(MIDDLE);
        CASE_MOUSE_BUTTONS(WHEEL_UP);
        CASE_MOUSE_BUTTONS(WHEEL_DOWN);
        CASE_MOUSE_BUTTONS(POSITION);
        CASE_MOUSE_BUTTONS(DRAG);
        CASE_MOUSE_BUTTONS(SIZE);
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const MOUSE_ACTION &action) {
#define CASE_MOUSE_ACTION(key) \
    case MOUSE_ACTION::key:    \
        return os << #key;
    switch (action) {
        CASE_MOUSE_ACTION(NONE);
        CASE_MOUSE_ACTION(PRESSED);
        CASE_MOUSE_ACTION(RELEASED);
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const KeyboardEvent &event) {
    os << "Keyboard: " << event.key;
    if (event.key == KEYS::CHARACTER) os << " ('" << event.character << "')";
    return os;
}

std::ostream &operator<<(std::ostream &os, const MouseEvent &event) {
    os << "Mouse: " << event.button << " at (" << event.position.x << ", " << event.position.y << ") " << event.action;
    return os;
}

std::ostream &operator<<(std::ostream &os, const PasteEvent &event) {
    os << "Paste: " << event.text;
    return os;
}

std::ostream &operator<<(std::ostream &os, const InputEvent &event) {
    switch (event.type) {
        case InputEvent::type_t::Keyboard:
            os << event.as.keyboardEvent;
            break;
        case InputEvent::type_t::Mouse:
            os << event.as.mouseEvent;
            break;
        case InputEvent::type_t::Paste:
            os << event.as.pasteEvent;
            break;
    }
    return os;
}

bool Input::have_to_read() {
    struct pollfd pollfd = {STDIN_FILENO, POLLIN, 0};
    bool ret = poll(&pollfd, 1, 0) == 1;
    debug_msg("Have to read: " << ret);
    return ret;
}

int Input::get_byte() {
    int r;
    unsigned char c;
    if ((r = read(STDIN_FILENO, &c, sizeof(c))) < 0) return r;
    debug_msg("Read byte: " << (char)c);
    return c;
}

void Input::handle_key(char byte) {
    if (byte >= 'a' && byte <= 'z') {
        add_event((char)byte);
    } else if (byte >= 'A' && byte <= 'Z') {
        add_event((char)byte);
    } else if (byte >= '0' && byte <= '9') {
        add_event((char)byte);
    } else if (byte == ' ') {
        add_event(KEYS::SPACE);
    } else if (byte == '\n' || byte == '\r') {
        add_event(KEYS::ENTER);
    } else if (byte == '\t') {
        add_event(KEYS::TAB);
    } else if (byte == '\b') {
        add_event(KEYS::BACKSPACE);
    } else if (byte == '\x7F') {
        add_event(KEYS::DELETE);
    } else if (byte == '\x1B') {
        add_event(KEYS::ESCAPE);
    }
}

void Input::process_input() {
    while (have_to_read()) {
        int byte_int = get_byte();
        if (byte_int == -1) break;
        char byte = (char)byte_int;

        switch (m_state) {
            case State::NORMAL:
                if (byte == '\x1B' && have_to_read()) {
                    m_state = State::ESC;
                } else {
                    handle_key(byte);
                }
                break;
            case State::ESC:
                if (byte == '[') {
                    m_state = State::CSI;
                    m_buffer.clear();
                } else {
                    add_event(KEYS::ESCAPE);
                    m_state = State::NORMAL;
                    handle_key(byte);
                }
                break;
            case State::CSI:
                if (m_buffer.empty()) {
                    if (byte == 'A') {
                        add_event(KEYS::UP);
                        m_state = State::NORMAL;
                    } else if (byte == 'B') {
                        add_event(KEYS::DOWN);
                        m_state = State::NORMAL;
                    } else if (byte == 'C') {
                        add_event(KEYS::RIGHT);
                        m_state = State::NORMAL;
                    } else if (byte == 'D') {
                        add_event(KEYS::LEFT);
                        m_state = State::NORMAL;
                    } else if (byte == '<') {
                        m_state = State::MOUSE;
                    } else if (byte == '2') {
                        m_buffer += byte;
                    } else {
                        // Unknown CSI, reset
                        m_state = State::NORMAL;
                    }
                } else {
                    // In '2...' sequence
                    m_buffer += byte;
                    if (m_buffer == "2~") {
                        add_event(KEYS::INSERT);
                        m_buffer.clear();
                        m_state = State::NORMAL;
                    } else if (m_buffer == "200~") {
                        m_buffer.clear();
                        m_state = State::PASTE_CONTENT;
                    } else if (m_buffer.size() > 4) {
                        m_buffer.clear();
                        m_state = State::NORMAL;
                    }
                }
                break;
            case State::MOUSE:
                m_buffer += byte;
                if (byte == 'M' || byte == 'm') {
                    process_mouse_input();
                    m_buffer.clear();
                    m_state = State::NORMAL;
                }
                break;
            case State::PASTE_CONTENT:
                m_buffer += byte;
                if (m_buffer.size() >= 6 && m_buffer.compare(m_buffer.size() - 6, 6, "\033[201~") == 0) {
                    std::string content = m_buffer.substr(0, m_buffer.size() - 6);
                    add_event(std::string_view(content));
                    m_buffer.clear();
                    m_state = State::NORMAL;
                }
                break;
            default:
                m_state = State::NORMAL;
                break;
        }
    }

#ifdef DEBUG
    debug_msg("Input events: " << m_events.size());
    for (auto &event : m_events) {
        debug_msg(event);
    }
#endif
}

// Parses the buffer accumulated in MOUSE state
void Input::process_mouse_input() {
    if (m_buffer.empty()) return;
    std::string_view buffer_view(m_buffer);

    MOUSE_BUTTONS button;
    size_t semicolon = buffer_view.find(';');
    if (semicolon == std::string_view::npos) return;

    std::string_view key_str = buffer_view.substr(0, semicolon);
    if (key_str == "0") {
        button = MOUSE_BUTTONS::LEFT;
    } else if (key_str == "1") {
        button = MOUSE_BUTTONS::MIDDLE;
    } else if (key_str == "2") {
        button = MOUSE_BUTTONS::RIGHT;
    } else if (key_str == "35") {
        button = MOUSE_BUTTONS::POSITION;
    } else if (key_str == "32" || key_str == "34") {
        button = MOUSE_BUTTONS::DRAG;
    } else if (key_str == "64") {
        button = MOUSE_BUTTONS::WHEEL_UP;
    } else if (key_str == "65") {
        button = MOUSE_BUTTONS::WHEEL_DOWN;
    } else {
        return;
    }

    MOUSE_ACTION action;
    if (buffer_view.back() == 'm') {
        action = MOUSE_ACTION::RELEASED;
    } else {
        action = MOUSE_ACTION::PRESSED;
    }

    MouseEvent mouseEvent = {button, {0, 0}, action};

    std::string_view pos_str = buffer_view.substr(semicolon + 1, buffer_view.size() - semicolon - 2);
    std::string_view remainder = buffer_view.substr(semicolon + 1);
    if (remainder.empty()) return;
    remainder.remove_suffix(1);  // Remove M/m

    size_t second_semicolon = remainder.find(';');
    if (second_semicolon == std::string_view::npos) return;

    std::string_view x_str = remainder.substr(0, second_semicolon);
    std::string_view y_str = remainder.substr(second_semicolon + 1);

    if (std::from_chars(x_str.data(), x_str.data() + x_str.size(), mouseEvent.position.x).ec != std::errc()) {
        return;
    }
    if (std::from_chars(y_str.data(), y_str.data() + y_str.size(), mouseEvent.position.y).ec != std::errc()) {
        return;
    }
    mouseEvent.position.x -= 1;
    mouseEvent.position.y -= 1;
    add_event(mouseEvent);
    m_mouse_position = mouseEvent.position;
    m_mouse_state[static_cast<int>(mouseEvent.button)] = mouseEvent.action;
}

bool Input::is_key_pressed(char c) {
    for (auto &event : m_events) {
        if (event.type == InputEvent::type_t::Keyboard && event.as.keyboardEvent.key == KEYS::CHARACTER &&
            event.as.keyboardEvent.character == c) {
            return true;
        }
    }
    return false;
}

bool Input::is_key_pressed(KEYS key) {
    for (auto &event : m_events) {
        if (event.type == InputEvent::type_t::Keyboard && event.as.keyboardEvent.key == key) {
            return true;
        }
    }
    return false;
}

bool Input::is_mouse_pressed(MOUSE_BUTTONS button) {
    return m_mouse_state[static_cast<int>(button)] == MOUSE_ACTION::PRESSED;
}

bool Input::is_mouse_released(MOUSE_BUTTONS button) {
    return m_mouse_state[static_cast<int>(button)] == MOUSE_ACTION::RELEASED;
}

MousePosition Input::get_mouse_position() { return m_mouse_position; }

bool Input::is_scroll_down() {
    for (auto &event : m_events) {
        if (event.type == InputEvent::type_t::Mouse && event.as.mouseEvent.button == MOUSE_BUTTONS::WHEEL_DOWN) {
            return true;
        }
    }
    return false;
}

bool Input::is_scroll_up() {
    for (auto &event : m_events) {
        if (event.type == InputEvent::type_t::Mouse && event.as.mouseEvent.button == MOUSE_BUTTONS::WHEEL_UP) {
            return true;
        }
    }
    return false;
}
}  // namespace TUIE