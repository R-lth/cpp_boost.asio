#include "ChatSession.h"
#include "ChatServer.h"

#include <istream>
#include <utility>

ChatSession::ChatSession(tcp::socket socket, ChatServer& server)
    : socket_(std::move(socket)), server_(server) {}

void ChatSession::start() {
    read();
}

void ChatSession::read() {
    auto self = shared_from_this(); // Keeps this session alive while the read is pending.
    asio::async_read_until(socket_, readBuffer_, '\n',
        [self](const boost::system::error_code& error, std::size_t) {
            if (error) {
                self->disconnect();
                return;
            }

            std::istream input(&self->readBuffer_);
            std::string line;
            std::getline(input, line);
            self->server_.broadcast(line + "\n");
            self->read();
        });
}

void ChatSession::deliver(const std::string& message) {
    const bool writeInProgress = !writeQueue_.empty();
    writeQueue_.push_back(message); // The queue owns the bytes until async_write completes.
    if (!writeInProgress) {
        write();
    }
}

void ChatSession::write() {
    auto self = shared_from_this(); // Also keeps writeQueue_.front() alive.
    asio::async_write(socket_, asio::buffer(writeQueue_.front()),
        [self](const boost::system::error_code& error, std::size_t) {
            if (error) {
                self->disconnect();
                return;
            }

            self->writeQueue_.pop_front();
            if (!self->writeQueue_.empty()) {
                self->write();
            }
        });
}

void ChatSession::disconnect() {
    if (disconnected_) {
        return;
    }
    disconnected_ = true;
    boost::system::error_code ignored;
    socket_.close(ignored);
    server_.leave(shared_from_this());
}
