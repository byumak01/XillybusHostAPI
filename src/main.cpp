#include <Server.h>
#include <iostream>

std::string usage() {
    return "./binary arg1, where arg1 is either host or client.";
}

int main([[maybe_unused]] int argc, char* argv[]) {
    if (const std::string arg = argv[1]; arg == "host") {
        std::cout << "Hello from the host" << std::endl;
        Server server;
        return server.run();
    } else if (arg == "client") {
        std::cout << "Hello from the client" << std::endl;
    } else if (arg == "help") {
        std::cout << usage() << std::endl;
        return 0;
    } else {
        throw std::runtime_error(usage());
    }

    return 0;
}
