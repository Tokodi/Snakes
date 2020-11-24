#pragma once

#include <deque>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/asio.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "common/connection.h"
#include "common/ts_queue.h"
#include "common/typedefs.h"
#include "common/utils.h"

namespace net {
namespace server {

template<class proto_message_t>
class proto_server_t {
  public:
    proto_server_t(const uint16_t port);
    virtual ~proto_server_t();

    bool start();

    void sendMessageToClient(std::shared_ptr<common::connection_t<proto_message_t>> connection, const proto_message_t& message);
    void broadcastMessage(const proto_message_t& message);

  private:
    void stop();
    void waitForConnection();
    void startMessageProcessingLoop();

    virtual void onConnectionClose(const std::shared_ptr<const common::connection_t<proto_message_t>> connection) = 0;
    virtual void onMessageReceive(const std::shared_ptr<const common::owned_message_t<proto_message_t>> message) = 0;

  private:
    uint32_t connectionId = 1;

    boost::asio::io_context _ioContext;
    boost::asio::ip::tcp::acceptor _acceptor;
    std::thread _ioContextThread;

    std::deque<std::shared_ptr<common::connection_t<proto_message_t>>> _connections;
    common::ts_queue<common::owned_raw_message_t<proto_message_t>> _inputMessageQueue;
    std::thread _messageProcessingThread;
};

template<class proto_message_t>
proto_server_t<proto_message_t>::proto_server_t(const uint16_t port)
    : _acceptor(_ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
}

template<class proto_message_t>
proto_server_t<proto_message_t>::~proto_server_t() {
    stop();
}

template<class proto_message_t>
bool proto_server_t<proto_message_t>::start() {
    try {
        waitForConnection();
        _ioContextThread = std::thread([this]() { _ioContext.run(); });
        _messageProcessingThread = std::thread([this]() { startMessageProcessingLoop(); });
    } catch(std::exception& e) {
        std::cerr << "[Server] Exception caught at start: " << e.what() << std::endl;
        return false;
    }

    std::cout << "[Server] Started!" << std::endl;

    return true;
}

template<class proto_message_t>
void proto_server_t<proto_message_t>::sendMessageToClient(std::shared_ptr<common::connection_t<proto_message_t>> connection, const proto_message_t& message) {
    if (connection && connection->isOpen()) {
        connection->send(message);
    } else {
        onConnectionClose(connection);
        connection.reset();
        _connections.erase(std::remove(_connections.begin(), _connections.end(), connection), _connections.end());
    }
}

template<class proto_message_t>
void proto_server_t<proto_message_t>::broadcastMessage(const proto_message_t& message) {
    bool invalidConnectionsFound = false;
    for (auto connection : _connections) {
        if (connection && connection->isOpen()) {
            connection->send(message);
        } else {
            onConnectionClose(connection);
            connection.reset();
            invalidConnectionsFound = true;
        }
    }

    if (invalidConnectionsFound) {
        _connections.erase(std::remove(_connections.begin(), _connections.end(), nullptr), _connections.end());
    }
}

template<class proto_message_t>
void proto_server_t<proto_message_t>::stop() {
    _ioContext.stop();
    if (_ioContextThread.joinable())
        _ioContextThread.join();

    // TODO: How to shut down infinity loop?
    if (_messageProcessingThread.joinable())
        _messageProcessingThread.join();

    std::cout << "[Server] Stopped!" << std::endl;
}

template<class proto_message_t>
void proto_server_t<proto_message_t>::waitForConnection() {
    _acceptor.async_accept(
        [this](std::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "[Server] New connection: " << socket.remote_endpoint() << std::endl;
                std::shared_ptr<common::connection_t<proto_message_t>> newConnection =
                    std::make_shared<common::connection_t<proto_message_t>> (_ioContext, std::move(socket), _inputMessageQueue, connectionId++);

                newConnection->startProcessing();
                _connections.push_back(std::move(newConnection));
            } else {
                std::cerr << "[Server] Error occured while accepting client: " << ec.message() << std::endl;
            }

            waitForConnection();
        }
    );
}

template<class proto_message_t>
void proto_server_t<proto_message_t>::startMessageProcessingLoop() {
    std::shared_ptr<common::owned_message_t<proto_message_t>> incomingMessage;
    while (true) {
        _inputMessageQueue.wait();

        incomingMessage =
            std::make_shared<common::owned_message_t<proto_message_t>>(_inputMessageQueue.front().ownerConnection);
        google::protobuf::io::ArrayInputStream ais(_inputMessageQueue.front().msg.data(), _inputMessageQueue.front().msg.size());
        google::protobuf::io::CodedInputStream cis(&ais);
        if (incomingMessage->msg.ParseFromCodedStream(&cis)) {
            if (!cis.ConsumedEntireMessage()) {
                std::cerr << "[Server] Could not parse entire message" << std::endl;
                throw; // TODO
            }
            onMessageReceive(incomingMessage);
        } else {
            std::cerr << "[Server] Message could not be processed: "
                      << _inputMessageQueue.front()
                      << std::endl;
        }
        incomingMessage.reset();
        _inputMessageQueue.pop_front();
    }
}

} // ns server
} // ns net
