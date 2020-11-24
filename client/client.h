#pragma once

#include <memory>
#include <thread>

#include "common/game_model/game.h"
#include "message.pb.h"
#include "net/client/client.h"
#include "ui/ncurses_game_view.h"

namespace client {

class client_t {
  public:
    client_t(const std::string& host, const uint16_t port);
    ~client_t();

    void login(const std::string& userName);
    void show_ui();

  private:
    void processIncomingEvents();

  private:
    std::unique_ptr<net::client::proto_client_t<snakes::common_msg_t>> _net_client;
    std::thread _messageProcessorThread;

    common::game_model::game_t _game;
    std::shared_ptr<ui::ncurses_view> _view;
};

} // ns client
