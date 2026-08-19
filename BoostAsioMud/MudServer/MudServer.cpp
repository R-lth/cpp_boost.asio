#include "MudServer.h"
#include "MudSession.h"

#include <iostream>

MudServer::MudServer(asio::io_context& ioContext, unsigned short port)
    : acceptor_(ioContext, tcp::endpoint(tcp::v4(), port)) {
    accept();
}

void MudServer::handleCommand(const std::shared_ptr<MudSession>& session,
                              const std::string& command) {
    CommandResult result = world_.execute(session->playerId(), command);
    deliver(result.messages);
    if (result.quit) {
        session->finish();
    }
}

void MudServer::leave(const std::shared_ptr<MudSession>& session) {
    if (sessions_.erase(session) == 0) {
        return;
    }

    deliver(world_.removePlayer(session->playerId()));
    std::cout << session->playerName() << " disconnected\n";
}

void MudServer::accept() {
    acceptor_.async_accept([this](const boost::system::error_code& error, tcp::socket socket) {
        if (!error) {
            auto session = std::make_shared<MudSession>(std::move(socket), *this, nextPlayerId_++);
            join(session);
            session->start();
        }
        if (error == asio::error::operation_aborted) {
            return;
        }
        accept();
    });
}

void MudServer::join(const std::shared_ptr<MudSession>& session) {
    sessions_.insert(session); // The server owns every connected network session.
    deliver(world_.addPlayer(session->playerId()));
    std::cout << session->playerName() << " connected\n";
}

void MudServer::deliver(const std::vector<GameMessage>& messages) {
    for (const GameMessage& message : messages) {
        for (const auto& session : sessions_) {
            if (session->playerId() == message.playerId) {
                session->deliver(message.text);
                break;
            }
        }
    }
}
