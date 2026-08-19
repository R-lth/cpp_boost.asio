#include "MudClient.h"

#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    try {
#ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
#endif
        asio::io_context ioContext;
        auto client = std::make_shared<MudClient>(ioContext);
        client->connect("127.0.0.1", 7777);

        std::thread ioThread([&ioContext] { ioContext.run(); });
        std::string line;
        while (std::cout << "> " && std::getline(std::cin, line)) {
            if (line.size() > 1000) {
                std::cout << "명령어가 너무 깁니다.\n";
                continue;
            }
            if (line == "/quit") {
                line = "quit";
            }
            client->send(line);
            if (line == "quit" || line == "종료" || line == "나가기") {
                break; // The server closes after sending the final response.
            }
        }

        if (!std::cin) {
            client->close();
        }
        ioThread.join();
    } catch (const std::exception& error) {
        std::cerr << "클라이언트 오류: " << error.what() << '\n';
        return 1;
    }
}
