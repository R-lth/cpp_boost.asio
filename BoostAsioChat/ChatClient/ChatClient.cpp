#include "ChatClient.h"

#include <iostream>
#include <istream>
#include <utility>

ChatClient::ChatClient(asio::io_context& ioContext)
    : ioContext_(ioContext), socket_(ioContext) {}

void ChatClient::connect(const std::string& host, unsigned short port) {
    tcp::resolver resolver(ioContext_);
    asio::connect(socket_, resolver.resolve(host, std::to_string(port)));
    std::cout << "Connected to server.\n\n";
    read();
}

void ChatClient::send(std::string message) {
    message += '\n';
    auto self = shared_from_this();
    asio::post(ioContext_, [self, message = std::move(message)]() mutable {
        const bool writeInProgress = !self->writeQueue_.empty();
        self->writeQueue_.push_back(std::move(message));
        if (!writeInProgress) {
            self->write();
        }
    });
}

void ChatClient::close() {
    auto self = shared_from_this();
    asio::post(ioContext_, [self] {
        boost::system::error_code ignored;
        self->socket_.close(ignored);
    });
}

void ChatClient::read() {
    auto self = shared_from_this();
    asio::async_read_until(socket_, readBuffer_, '\n',
        [self](const boost::system::error_code& error, std::size_t) {
            if (error) {
                return;
            }

            std::istream input(&self->readBuffer_);
            std::string line;
            std::getline(input, line);
            std::cout << line << '\n';
            self->read();
        });
}

void ChatClient::write() {
    auto self = shared_from_this();
    asio::async_write(socket_, asio::buffer(writeQueue_.front()),
        [self](const boost::system::error_code& error, std::size_t) {
            if (error) {
                return;
            }

            self->writeQueue_.pop_front();
            if (!self->writeQueue_.empty()) {
                self->write();
            }
        });
}
