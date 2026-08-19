#include "ChatServer.h"
#include "ChatSession.h"

#include <iostream>

ChatServer::ChatServer(asio::io_context& ioContext, unsigned short port)
    : acceptor_(ioContext, tcp::endpoint(tcp::v4(), port)) {
    accept();
}

void ChatServer::join(const std::shared_ptr<ChatSession>& session) {
    sessions_.insert(session); // The server owns every active session.
    std::cout << "Client Connected\n";
}

void ChatServer::leave(const std::shared_ptr<ChatSession>& session) {
    if (sessions_.erase(session) != 0) {
        std::cout << "Client Disconnected\n";
    }
}

void ChatServer::broadcast(const std::string& message) {
    for (const auto& session : sessions_) {
        session->deliver(message);
    }
}

void ChatServer::accept() {
    acceptor_.async_accept([this](const boost::system::error_code& error, tcp::socket socket) {
        if (!error) {
            auto session = std::make_shared<ChatSession>(std::move(socket), *this);
            join(session);
            session->start();
        }

        accept();
    });
}
