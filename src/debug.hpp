#pragma once

#include <fstream>
#include <functional>

namespace TUIE {

// Because is a TUI the debug messages can mess up the display, so its needs to go to a file instead of cerr
class DebugStream {
   public:
    DebugStream() : file("debug.txt", std::ios::out | std::ios::trunc) {}
    ~DebugStream() { file.close(); }
    std::ofstream file;

    static DebugStream& instance() {
        static DebugStream instance;
        return instance;
    }
};

#ifdef DEBUG
#define debug_msg(message) DebugStream::instance().file << message << std::endl;
#else
#define debug_msg(message)
#endif

template <typename T>
struct __defer_impl {
    T func;
    __defer_impl(T f) : func(f) {}
    ~__defer_impl() { func(); }
};

// This macro set the line number to the name of the variable to avoid name collisions
#define __defer_name2(line) _defer_obj_line_##line
#define __defer_name(line)  __defer_name2(line)
#define defer(func)         __defer_impl __defer_name(__LINE__)(func);

}  // namespace TUIE