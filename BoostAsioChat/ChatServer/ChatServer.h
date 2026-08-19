#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <set>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class ChatSession;

class ChatServer {
public:
    ChatServer(asio::io_context& ioContext, unsigned short port);

    void join(const std::shared_ptr<ChatSession>& session);
    void leave(const std::shared_ptr<ChatSession>& session);
    void broadcast(const std::string& message);

private:
    void accept();

    tcp::acceptor acceptor_;
    std::set<std::shared_ptr<ChatSession>> sessions_;
};
