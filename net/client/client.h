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
    ~proto_client_t();

    bool connect(const std::string& host, const uint16_t port);

    void sendMessage(const proto_message_t& message);
    const std::shared_ptr<common::owned_message_t<proto_message_t>> getEvent();

  private:
    boost::asio::io_context _ioContext;
    std::thread _ioContextThread;

    std::shared_ptr<common::connection_t<proto_message_t>> _connection;
    common::ts_queue<common::owned_raw_message_t<proto_message_t>> _inputMessageQueue;
};

template<class proto_message_t>
proto_client_t<proto_message_t>::~proto_client_t() {
    _ioContext.stop();
    if (_ioContextThread.joinable()) {
        _ioContextThread.join();
    }
}

template<class proto_message_t>
bool proto_client_t<proto_message_t>::connect(const std::string& host, const uint16_t port) {
    try {
        boost::asio::ip::tcp::resolver resolver(_ioContext);
        boost::asio::ip::tcp::resolver::results_type endpoint = resolver.resolve(host, std::to_string(port));

        _connection = std::make_shared<common::connection_t<proto_message_t>>(_ioContext, boost::asio::ip::tcp::socket(_ioContext), _inputMessageQueue, 0);
        _connection->connectToEndpoint(endpoint);
        // TODO: return value for connectToEndpoint and if success -> call start processing here

        _ioContextThread = std::thread([this]() { _ioContext.run(); });
    } catch (std::exception& e) {
        std::cerr << "[Client] Connect exception: " << e.what() << std::endl;
        return false;
    }

    return true;
}

template<class proto_message_t>
void proto_client_t<proto_message_t>::sendMessage(const proto_message_t& message) {
    if (_connection && _connection->isOpen()) {
        _connection->send(message);
    } else {
        std::cerr << "[Client] Could not send message. Not connected to server." <<  std::endl;
    }
}

// TODO: This is common code with server
template<class proto_message_t>
const std::shared_ptr<common::owned_message_t<proto_message_t>> proto_client_t<proto_message_t>::getEvent() {
    _inputMessageQueue.wait();

    std::shared_ptr<common::owned_message_t<proto_message_t>> incomingMessage =
        std::make_shared<common::owned_message_t<proto_message_t>>(_inputMessageQueue.front().ownerConnection);
    google::protobuf::io::ArrayInputStream ais(_inputMessageQueue.front().msg.data(), _inputMessageQueue.front().msg.size());
    google::protobuf::io::CodedInputStream cis(&ais);
    if (incomingMessage->msg.ParseFromCodedStream(&cis)) {
        incomingMessage->ownerConnection = _inputMessageQueue.front().ownerConnection;
        if (!cis.ConsumedEntireMessage()) {
            throw; // TODO
        }
    } else {
        std::cerr << "[Client] Message could not be processed: "
                  << _inputMessageQueue.front()
                  << std::endl;
    }
    _inputMessageQueue.pop_front();

    return incomingMessage;
}

} // ns client
} // ns net
