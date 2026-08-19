#include "ChatServer.h"

#include <exception>
#include <iostream>

int main() {
    try {
        asio::io_context ioContext;
        ChatServer server(ioContext, 7777);

        std::cout << "Chat Server Started\n";
        std::cout << "Port : 7777\n";
        ioContext.run();
    } catch (const std::exception& error) {
        std::cerr << "Server error: " << error.what() << '\n';
        return 1;
    }
}
