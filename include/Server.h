#pragma once

#include "XillybusManager.h"
#include "headers.h"

using namespace boost::interprocess;

class Server {
public:
    static int run();

private:
    // Core methods
    static void setup_signals();
    static void main_loop();
    static void do_work();
    static void cleanup();

    // Xillybus manager
    inline static std::optional<XillybusManager> xillybus_manager_;
};

class Client {};
