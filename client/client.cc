#include "client.h"

#include <iostream>

using position_t = std::pair<uint32_t, uint32_t>;

namespace client {

client_t::client_t(const std::string& host, const uint16_t port) {
    if (!connectToServer(host, port)) {
        std::cerr << "[GameClient] Server is unavailable!" << std::endl;
    }
}

void client_t::onMessageReceive(const std::shared_ptr<const net::common::owned_message_t<snakes::common_msg_t>> message) {
    if (!message) {
        std::cerr << "[GameClient] Incoming message is nullptr! Terminating." << std::endl;
        return;
    }

    switch (message->msg.id()) {
        case 2:
            // Got table modifications
            for (auto& field : message->msg.field_change().position()) {
                switch (field.field_type()) {
                    case snakes::field_t::FOOD:
                        _game.setTableField(position_t(field.x(), field.y()), 'F');
                        break;
                    case snakes::field_t::SNAKE:
                        _game.setTableField(position_t(field.x(), field.y()), '0' + field.id());
                        break;
                    case snakes::field_t::EMPTY:
                        _game.setTableField(position_t(field.x(), field.y()), '0');
                        break;
                }
            }
            break;
        case 3:
            // Step game
            _game.getTable()->debugPrint();
            break;
        default:
            std::cerr << "[GameClient] Unknown event id" << std::endl;
            break;
    }
}

void client_t::login(const std::string& userName) {
    snakes::common_msg_t loginMsg;
    loginMsg.set_id(1);
    loginMsg.mutable_login()->set_username(userName);

    sendMessageToServer(loginMsg);
}

} // ns client
