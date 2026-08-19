#include "MudClient.h"

#include <iostream>
#include <istream>
#include <utility>

namespace {
constexpr std::size_t kMaxQueuedBytes = 64 * 1024;
}

MudClient::MudClient(asio::io_context& ioContext)
    : ioContext_(ioContext), socket_(ioContext), readBuffer_(4096) {}

void MudClient::connect(const std::string& host, unsigned short port) {
    tcp::resolver resolver(ioContext_);
    asio::connect(socket_, resolver.resolve(host, std::to_string(port)));
    std::cout << "MUD 서버에 연결되었습니다.\n\n";
    read();
}

void MudClient::send(std::string command) {
    command += '\n';
    const std::size_t commandSize = command.size();
    if (commandSize > kMaxQueuedBytes) {
        std::cerr << "명령어가 너무 깁니다.\n";
        return;
    }
    std::size_t pending = pendingBytes_.load();
    do {
        if (closed_ || pending > kMaxQueuedBytes - commandSize) {
            std::cerr << "대기 중인 입력이 너무 많거나 연결이 종료되었습니다.\n";
            return;
        }
    } while (!pendingBytes_.compare_exchange_weak(pending, pending + commandSize));

    auto self = shared_from_this();
    asio::post(ioContext_, [self, command = std::move(command), commandSize]() mutable {
        if (self->closed_) {
            self->pendingBytes_ -= commandSize;
            return;
        }
        const bool writeInProgress = !self->writeQueue_.empty();
        self->writeQueue_.push_back(std::move(command));
        if (!writeInProgress) {
            self->write();
        }
    });
}

void MudClient::close() {
    if (closed_.exchange(true)) {
        return;
    }
    auto self = shared_from_this();
    asio::post(ioContext_, [self] {
        boost::system::error_code ignored;
        self->socket_.close(ignored);
    });
}

void MudClient::read() {
    auto self = shared_from_this();
    asio::async_read_until(socket_, readBuffer_, '\n',
        [self](const boost::system::error_code& error, std::size_t) {
            if (error) {
                if (!self->closed_.exchange(true)) {
                    std::cerr << "서버와의 연결이 끊어졌습니다.\n";
                }
                return;
            }

            std::istream input(&self->readBuffer_);
            std::string line;
            std::getline(input, line);
            std::cout << line << '\n';
            self->read();
        });
}

void MudClient::write() {
    auto self = shared_from_this();
    asio::async_write(socket_, asio::buffer(writeQueue_.front()),
        [self](const boost::system::error_code& error, std::size_t) {
            if (error) {
                if (!self->closed_.exchange(true)) {
                    std::cerr << "서버와의 연결이 끊어졌습니다.\n";
                }
                return;
            }

            self->pendingBytes_ -= self->writeQueue_.front().size();
            self->writeQueue_.pop_front();
            if (!self->writeQueue_.empty()) {
                self->write();
            }
        });
}
