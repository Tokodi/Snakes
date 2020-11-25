#pragma once

#include <cstdint>
#include <memory>
#include <thread>

#include "common/game_model/game.h"
#include "message.pb.h"
#include "net/client/client.h"
#include "ui/ncurses_game_view.h"

namespace client {

class client_t final : public net::client::proto_client_t<snakes::common_msg_t> {
  public:
    client_t(const std::string& host, const uint16_t port);
    // TODO: Is parents desctructor called?

    void login(const std::string& userName);

    void listenForKeyboardInput();

  private:
    void onMessageReceive(const std::shared_ptr<const net::common::owned_message_t<snakes::common_msg_t>> message) final;

  private:
    std::thread _inputListenerThread;

    common::game_model::game_t _game;
    std::shared_ptr<ui::ncurses_view> _view;
};

} // ns client
