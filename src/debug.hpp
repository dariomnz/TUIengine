#pragma once

#include <fstream>

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

}  // namespace TUIE