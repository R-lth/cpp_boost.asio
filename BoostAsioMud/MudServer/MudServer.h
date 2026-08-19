#pragma once

#include "GameWorld.h"

#include <boost/asio.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class MudSession;

class MudServer {
public:
    MudServer(asio::io_context& ioContext, unsigned short port);

    void handleCommand(const std::shared_ptr<MudSession>& session, const std::string& command);
    void leave(const std::shared_ptr<MudSession>& session);

private:
    void accept();
    void join(const std::shared_ptr<MudSession>& session);
    void deliver(const std::vector<GameMessage>& messages);

    tcp::acceptor acceptor_;
    std::set<std::shared_ptr<MudSession>> sessions_;
    GameWorld world_;
    int nextPlayerId_ = 1;
};
