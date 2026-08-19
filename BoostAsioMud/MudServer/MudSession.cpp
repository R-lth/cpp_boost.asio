#include "MudSession.h"
#include "MudServer.h"

#include <istream>
#include <utility>

namespace {
constexpr std::size_t kMaxQueuedBytes = 64 * 1024;
}

MudSession::MudSession(tcp::socket socket, MudServer& server, int playerId)
    : socket_(std::move(socket)), server_(server), playerId_(playerId), readBuffer_(1024) {}

void MudSession::start() {
    read();
}

void MudSession::deliver(const std::string& message) {
    if (disconnected_ || finishing_) {
        return;
    }
    if (queuedBytes_ + message.size() > kMaxQueuedBytes) {
        // Closing makes the pending callback perform normal, idempotent cleanup later.
        boost::system::error_code ignored;
        socket_.close(ignored);
        return;
    }

    const bool writeInProgress = !writeQueue_.empty();
    writeQueue_.push_back(message); // Owns the bytes until async_write completes.
    queuedBytes_ += message.size();
    if (!writeInProgress) {
        write();
    }
}

void MudSession::finish() {
    finishing_ = true;
    if (writeQueue_.empty()) {
        disconnect();
    }
}

int MudSession::playerId() const {
    return playerId_;
}

std::string MudSession::playerName() const {
    return "Player" + std::to_string(playerId_);
}

void MudSession::read() {
    auto self = shared_from_this(); // Keeps the session alive while the read is pending.
    asio::async_read_until(socket_, readBuffer_, '\n',
        [self](const boost::system::error_code& error, std::size_t) {
            if (error) {
                self->disconnect();
                return;
            }

            std::istream input(&self->readBuffer_);
            std::string line;
            std::getline(input, line);
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            self->server_.handleCommand(self, line);
            if (!self->finishing_) {
                self->read();
            }
        });
}

void MudSession::write() {
    auto self = shared_from_this(); // Keeps both the session and its queued buffer alive.
    asio::async_write(socket_, asio::buffer(writeQueue_.front()),
        [self](const boost::system::error_code& error, std::size_t) {
            if (error) {
                self->disconnect();
                return;
            }

            self->queuedBytes_ -= self->writeQueue_.front().size();
            self->writeQueue_.pop_front();
            if (!self->writeQueue_.empty()) {
                self->write();
            } else if (self->finishing_) {
                self->disconnect();
            }
        });
}

void MudSession::disconnect() {
    if (disconnected_) {
        return;
    }
    disconnected_ = true;
    boost::system::error_code ignored;
    socket_.close(ignored);
    server_.leave(shared_from_this());
}
