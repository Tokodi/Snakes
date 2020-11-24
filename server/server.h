#pragma once

#include <memory>
#include <thread>

#include "common/game_model/game.h"
#include "message.pb.h"
#include "net/server/server.h"

namespace server {

class server_t final : public net::server::proto_server_t<snakes::common_msg_t> {
  public:
    server_t(const uint16_t port);
    ~server_t() {}

    void startGame();

  private:
    void processLoginMessage(const std::shared_ptr<const net::common::owned_message_t<snakes::common_msg_t>> loginMessage);

    void onConnectionClose(const std::shared_ptr<const net::common::connection_t<snakes::common_msg_t>> connection) final;
    void onMessageReceive(const std::shared_ptr<const net::common::owned_message_t<snakes::common_msg_t>> message) final;

  private:
    common::game_model::game_t _game;
};

} // ns server
