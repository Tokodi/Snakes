#pragma once

#include <iostream>
#include <type_traits>
#include <vector>

#include <boost/asio.hpp>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "ts_queue.h"
#include "typedefs.h"

namespace net {
namespace common {

template <class proto_message_t>
class connection_t : public std::enable_shared_from_this<connection_t<proto_message_t>> {
  public:
    connection_t(boost::asio::io_context& ioContext,
                 boost::asio::ip::tcp::socket socket,
                 ts_queue<owned_raw_message_t<proto_message_t>>& inputMessageQueue,
                 uint32_t id);
    ~connection_t() { close(); }

    void connectToEndpoint(const boost::asio::ip::tcp::resolver::results_type& endpoint);

    void startProcessing();
    void send(const proto_message_t& message);

    bool isOpen() const;
    uint32_t getId() const;

  private:
    void close();

    void receiveData();
    void onReceive(std::error_code ec, std::size_t bytesReceived);

    std::vector<uint8_t> serializeMessage(const proto_message_t& message);
    void writeMessage();

  private:
    // Counters
    std::atomic_int _receivedEventCounter = 0;
    std::atomic_int _receivedBytes = 0;
    std::atomic_int _sentEventCounter = 0;
    std::atomic_int _sentBytes = 0;

    boost::asio::io_context& _ioContext;
    boost::asio::ip::tcp::socket _socket;

    boost::asio::streambuf _buffer;
    ts_queue<std::vector<uint8_t>> _outputMessageQueue;
    ts_queue<owned_raw_message_t<proto_message_t>>& _inputMessageQueue;

    const uint32_t _id;
};

template <class proto_message_t>
connection_t<proto_message_t>::connection_t(boost::asio::io_context& ioContext,
                           boost::asio::ip::tcp::socket socket,
                           ts_queue<owned_raw_message_t<proto_message_t>>& inputMessageQueue,
                           uint32_t id)
    : _ioContext(ioContext)
    , _socket(std::move(socket))
    , _buffer(MAXIMUM_BUFFER_SIZE)
    , _inputMessageQueue(inputMessageQueue)
    , _id(id) {}


template <class proto_message_t>
void connection_t<proto_message_t>::connectToEndpoint(const boost::asio::ip::tcp::resolver::results_type& endpoint) {
    boost::asio::async_connect(_socket, endpoint,
        [this](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint) {
            if (!ec) {
                startProcessing();
            }
        }
    );
}

template <class proto_message_t>
void connection_t<proto_message_t>::startProcessing() {
    receiveData();
}

template <class proto_message_t>
void connection_t<proto_message_t>::send(const proto_message_t& message) {
    boost::asio::post(_ioContext, [this, message]() {
        bool _isMessageBeingWritten = !_outputMessageQueue.empty();
        _outputMessageQueue.push_back(std::move(serializeMessage(message)));
        if (!_isMessageBeingWritten) {
            writeMessage();
        }
    });
}

template <class proto_message_t>
bool connection_t<proto_message_t>::isOpen() const {
    return _socket.is_open();
}

template <class proto_message_t>
uint32_t connection_t<proto_message_t>::getId() const {
    return _id;
}

template <class proto_message_t>
void connection_t<proto_message_t>::close() {
    if (_socket.is_open()) {
        boost::asio::post(_ioContext, [this](){ _socket.close(); });
        std::cout << "[Connection] (" << _socket.remote_endpoint() << ") Received " << _receivedEventCounter << " events" << std::endl;
        std::cout << "[Connection] (" << _socket.remote_endpoint() << ") Received " << _receivedBytes << " bytes" << std::endl;
        std::cout << "[Connection] (" << _socket.remote_endpoint() << ") Sent " << _sentEventCounter << " events" << std::endl;
        std::cout << "[Connection] (" << _socket.remote_endpoint() << ") Sent " << _sentBytes << " bytes" << std::endl;
    }
}

template <class proto_message_t>
void connection_t<proto_message_t>::receiveData() {
    _socket.async_receive(_buffer.prepare(MAXIMUM_RECEIVE_SIZE), 0,
                          std::bind(&connection_t::onReceive,
                                    std::enable_shared_from_this<connection_t>::shared_from_this(),
                                    std::placeholders::_1, std::placeholders::_2));
}

template <class proto_message_t>
void connection_t<proto_message_t>::onReceive(std::error_code ec, std::size_t bytesReceived) {
    if (!ec) {
        _receivedBytes += bytesReceived;

        if (bytesReceived == 0) {
            close();
            return;
        }

        _buffer.commit(bytesReceived);

        uint32_t messageLength;
        const uint8_t* data = static_cast<const uint8_t*>(_buffer.data().data());
        std::shared_ptr<google::protobuf::io::ArrayInputStream> ais = std::make_shared<google::protobuf::io::ArrayInputStream>(data, _buffer.size());
        std::shared_ptr<google::protobuf::io::CodedInputStream> cis = std::make_shared<google::protobuf::io::CodedInputStream>(ais.get());
        while(1) {
            if (_buffer.size() == 0) {
                break;
            }
            if (cis->ReadVarint32(&messageLength)) {
                if (_buffer.size() >= messageLength) {
                    ++_receivedEventCounter;
                    _inputMessageQueue.push_back(
                            owned_raw_message_t<proto_message_t>(std::enable_shared_from_this<connection_t<proto_message_t>>::shared_from_this(),
                                                                 std::vector<uint8_t>(data + 1, data + 1 + messageLength))
                    );
                    _buffer.consume(1 + messageLength);
                    data = static_cast<const uint8_t*>(_buffer.data().data());
                    cis->Skip(messageLength);
                } else {
                    std::cerr << "[Connection] (" << _socket.remote_endpoint() << ") Not enough data. Waiting for some more." << std::endl;
                    break;
                }
            } else {
                std::cerr << "[Connection] (" << _socket.remote_endpoint() << ") Could not read varint." << std::endl;
                break;
            }
        }
        receiveData();
    } else {
        std::cout << "[Connection] (" << _socket.remote_endpoint() << ") Receive error: " << ec.message() << std::endl;
        close();
    }
}

template <class proto_message_t>
std::vector<uint8_t> connection_t<proto_message_t>::serializeMessage(const proto_message_t& message) {
    std::vector<uint8_t> messageBuffer;
    messageBuffer.resize(message.ByteSizeLong() + 1); // +1 for warint. Max message size: ...

    google::protobuf::io::ArrayOutputStream aos(messageBuffer.data(), messageBuffer.size());
    google::protobuf::io::CodedOutputStream cos(&aos);

    cos.WriteVarint32(message.ByteSizeLong());
    message.SerializeToCodedStream(&cos);

    return messageBuffer;
}

template <class proto_message_t>
void connection_t<proto_message_t>::writeMessage() {
    boost::asio::async_write(_socket, boost::asio::buffer(_outputMessageQueue.front().data(), _outputMessageQueue.front().size()),
        [this](std::error_code ec, std::size_t bytesTransfered) {
            if (!ec) {
                ++_sentEventCounter;
                _outputMessageQueue.pop_front();

                if (!_outputMessageQueue.empty()) {
                    writeMessage();
                }
            } else {
                std::cout << "[Connection] (" << _socket.remote_endpoint() << ") Write error: " << ec.message() << std::endl;
                _socket.close();
            }
        }
    );
}

} // ns common
} // ns net
