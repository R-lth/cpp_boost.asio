#pragma once

#include <boost/asio.hpp>
#include <atomic>
#include <deque>
#include <memory>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class MudClient : public std::enable_shared_from_this<MudClient> {
public:
    explicit MudClient(asio::io_context& ioContext);

    void connect(const std::string& host, unsigned short port);
    void send(std::string command);
    void close();

private:
    void read();
    void write();

    asio::io_context& ioContext_;
    tcp::socket socket_;
    asio::streambuf readBuffer_;
    std::deque<std::string> writeQueue_;
    std::atomic_size_t pendingBytes_{0};
    std::atomic_bool closed_{false};
};
