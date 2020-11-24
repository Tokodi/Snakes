#pragma once

#include <memory>
#include <thread>

#include "common/game_model/game.h"
#include "message.pb.h"
#include "net/server/server.h"

namespace server {

// TODO: Inherit from net_server, that way OnDisconnect can be overwritten...
class server_t {
  public:
    server_t(const uint16_t port);
    ~server_t();

    void start();

  private:
    void processIncomingEvents();

    void processLoginMessage(std::shared_ptr<net::common::owned_message_t<snakes::common_msg_t>> loginMessage);

  private:
    std::unique_ptr<net::server::proto_server_t<snakes::common_msg_t>> _net_server;
    std::thread _messageProcessorThread;

    common::game_model::game_t _game;
};

} // ns server
