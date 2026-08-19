#include "MudServer.h"

#include <exception>
#include <iostream>

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
        MudServer server(ioContext, 7777);
        std::cout << "MUD 서버가 시작되었습니다.\n포트: 7777\n";
        ioContext.run();
    } catch (const std::exception& error) {
        std::cerr << "서버 오류: " << error.what() << '\n';
        return 1;
    }
}
