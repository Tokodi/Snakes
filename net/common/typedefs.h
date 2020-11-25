#pragma once

#include <cstdint>
#include <memory>
#include <ostream>
#include <vector>

#include "connection.h"
#include "utils.h"

namespace net {
namespace common {

static constexpr std::size_t MAXIMUM_BUFFER_SIZE = 2048;
static constexpr std::size_t MAXIMUM_RECEIVE_SIZE = 128;

// TODO: Why does this need a forward declaration?
template<class proto_message_t>
class connection_t;

template<class proto_message_t>
struct owned_message_t {
    owned_message_t(std::shared_ptr<connection_t<proto_message_t>> connection)
        : ownerConnection(connection) {}

    std::shared_ptr<connection_t<proto_message_t>> ownerConnection = nullptr;
    proto_message_t  msg;
};

template<class proto_message_t>
struct owned_raw_message_t {
    owned_raw_message_t(std::shared_ptr<connection_t<proto_message_t>> connection, std::vector<uint8_t> msg)
        : ownerConnection(connection), msg(msg) {}

    std::shared_ptr<connection_t<proto_message_t>> ownerConnection = nullptr;
    std::vector<uint8_t> msg;

    friend std::ostream& operator<<(std::ostream& os, const owned_raw_message_t<proto_message_t>& msg) {
        os << utils::uint8_to_hex_string(msg.msg.data(), msg.msg.size());
        return os;
    }
};

} // ns common
} // ns net
