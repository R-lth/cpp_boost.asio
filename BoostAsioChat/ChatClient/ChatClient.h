#pragma once

#include <boost/asio.hpp>
#include <deque>
#include <memory>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class ChatClient : public std::enable_shared_from_this<ChatClient> {
public:
    explicit ChatClient(asio::io_context& ioContext);

    void connect(const std::string& host, unsigned short port);
    void send(std::string message);
    void close();

private:
    void read();
    void write();

    asio::io_context& ioContext_;
    tcp::socket socket_;
    asio::streambuf readBuffer_;
    std::deque<std::string> writeQueue_;
};
