#pragma once

#include <boost/asio.hpp>
#include <deque>
#include <memory>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class MudServer;

class MudSession : public std::enable_shared_from_this<MudSession> {
public:
    MudSession(tcp::socket socket, MudServer& server, int playerId);

    void start();
    void deliver(const std::string& message);
    void finish();
    int playerId() const;
    std::string playerName() const;

private:
    void read();
    void write();
    void disconnect();

    tcp::socket socket_;
    MudServer& server_; // Non-owning: main keeps the server alive while io_context runs.
    const int playerId_;
    asio::streambuf readBuffer_;
    std::deque<std::string> writeQueue_;
    std::size_t queuedBytes_ = 0;
    bool finishing_ = false;
    bool disconnected_ = false;
};
