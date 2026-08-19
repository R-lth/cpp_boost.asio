#include "ChatClient.h"

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

int main() {
    try {
        asio::io_context ioContext;
        auto client = std::make_shared<ChatClient>(ioContext);
        client->connect("127.0.0.1", 7777);

        std::thread ioThread([&ioContext] { ioContext.run(); });

        std::string line;
        while (std::cout << "> " && std::getline(std::cin, line)) {
            client->send(line);
        }

        client->close();
        ioThread.join();
    } catch (const std::exception& error) {
        std::cerr << "Client error: " << error.what() << '\n';
        return 1;
    }
}
