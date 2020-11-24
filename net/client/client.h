#pragma once

#include <iostream>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "common/connection.h"
#include "common/ts_queue.h"
#include "common/typedefs.h"

namespace net {
namespace client {

template<class proto_message_t>
class proto_client_t {
  public:
    proto_client_t() {}
    virtual ~proto_client_t();

  protected:
    bool connectToServer(const std::string& ip, const uint16_t port);
    void sendMessageToServer(const proto_message_t& message);

    virtual void onMessageReceive(const std::shared_ptr<const common::owned_message_t<proto_message_t>> message) = 0;

  private:
    void startMessageProcessingLoop();

  private:
    boost::asio::io_context _ioContext;
    std::thread _ioContextThread;

    std::shared_ptr<common::connection_t<proto_message_t>> _connection;
    common::ts_queue<common::owned_raw_message_t<proto_message_t>> _inputMessageQueue;
    std::thread _messageProcessingThread;
};

template<class proto_message_t>
proto_client_t<proto_message_t>::~proto_client_t() {
    _ioContext.stop();
    if (_ioContextThread.joinable()) {
        _ioContextThread.join();
    }

    // TODO: How to shut down infinity loop?
    if (_messageProcessingThread.joinable())
        _messageProcessingThread.join();
}

template<class proto_message_t>
bool proto_client_t<proto_message_t>::connectToServer(const std::string& ip, const uint16_t port) {
    try {
        boost::asio::ip::tcp::resolver resolver(_ioContext);
        boost::asio::ip::tcp::resolver::results_type endpoint = resolver.resolve(ip, std::to_string(port));

        _connection = std::make_shared<common::connection_t<proto_message_t>>(_ioContext, boost::asio::ip::tcp::socket(_ioContext), _inputMessageQueue, 0);
        _connection->connectToEndpoint(endpoint);
        // TODO: return value for connectToEndpoint and if success -> call start processing here

        _ioContextThread = std::thread([this]() { _ioContext.run(); });
        _messageProcessingThread = std::thread([this]() { startMessageProcessingLoop(); });
    } catch (std::exception& e) {
        std::cerr << "[Client] Connect exception: " << e.what() << std::endl;
        return false;
    }

    return true;
}

template<class proto_message_t>
void proto_client_t<proto_message_t>::sendMessageToServer(const proto_message_t& message) {
    if (_connection && _connection->isOpen()) {
        _connection->send(message);
    } else {
        std::cerr << "[Client] Could not send message. Not connected to server." <<  std::endl;
    }
}

// TODO: owned message is not needed here. Client only has 1 connection, and its the server
template<class proto_message_t>
void proto_client_t<proto_message_t>::startMessageProcessingLoop() {
    std::shared_ptr<common::owned_message_t<proto_message_t>> incomingMessage;
    while (true) {
        _inputMessageQueue.wait();

        incomingMessage =
            std::make_shared<common::owned_message_t<proto_message_t>>(_inputMessageQueue.front().ownerConnection);
        google::protobuf::io::ArrayInputStream ais(_inputMessageQueue.front().msg.data(), _inputMessageQueue.front().msg.size());
        google::protobuf::io::CodedInputStream cis(&ais);
        if (incomingMessage->msg.ParseFromCodedStream(&cis)) {
            if (!cis.ConsumedEntireMessage()) {
                std::cerr << "[Client] Could not parse entire message" << std::endl;
                throw; // TODO
            }
            onMessageReceive(incomingMessage);
        } else {
            std::cerr << "[Client] Message could not be processed: "
                      << _inputMessageQueue.front()
                      << std::endl;
        }
        incomingMessage.reset();
        _inputMessageQueue.pop_front();
    }
}

} // ns client
} // ns net
