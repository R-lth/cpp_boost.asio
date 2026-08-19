#pragma once

#include <boost/asio.hpp>
#include <deque>
#include <memory>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class ChatServer;

class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    ChatSession(tcp::socket socket, ChatServer& server);

    void start();
    void deliver(const std::string& message);

private:
    void read();
    void write();
    void disconnect();

    tcp::socket socket_;
    ChatServer& server_; // Non-owning: the server outlives its sessions.
    asio::streambuf readBuffer_;
    std::deque<std::string> writeQueue_;
    bool disconnected_ = false;
};
