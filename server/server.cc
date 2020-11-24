#include "server.h"

#include <iostream>

using position_t = std::pair<uint32_t, uint32_t>;

namespace server {

server_t::server_t(const uint16_t port) : net::server::proto_server_t<snakes::common_msg_t>(port) {
    _game.createFood();
    _game.getTable()->debugPrint();
}

void server_t::onConnectionClose(const std::shared_ptr<const net::common::connection_t<snakes::common_msg_t>> connection) {
    // TODO: Remove snake from table
    // TODO: Broadcast this...
}

void server_t::onMessageReceive(const std::shared_ptr<const net::common::owned_message_t<snakes::common_msg_t>> message) {
    if (!message) {
        std::cerr << "[GameServer] Incoming message is nullptr! Terminating." << std::endl;
        return;
    }

    switch (message->msg.id()) {
        case 1:
            processLoginMessage(message);
            break;
        default:
            std::cerr << "[GameServer] Unknown event id" << std::endl;
            break;
    }
}

void server_t::processLoginMessage(const std::shared_ptr<const net::common::owned_message_t<snakes::common_msg_t>> loginMessage) {
    std::cout << "[GameServer] Processing login message " << std::endl;
    position_t playerPosition = _game.placePlayerOnTable(loginMessage->ownerConnection->getId(), loginMessage->msg.login().username());

    // Broadcast new player position
    snakes::common_msg_t commonSnakeMsg;
    commonSnakeMsg.set_id(3);
    commonSnakeMsg.mutable_snake()->set_id(loginMessage->ownerConnection->getId());

    snakes::position_msg_t* position = commonSnakeMsg.mutable_snake()->add_snakepart();
    position->set_x(playerPosition.first);
    position->set_y(playerPosition.second);

    broadcastMessage(commonSnakeMsg);

    // Send already added snakes to new player
    // TODO: Sends duplicate of the newly added player
    for (const auto& snake : _game.getSnakes()) {
        snakes::common_msg_t commonSnakeMsg;
        commonSnakeMsg.set_id(3);
        commonSnakeMsg.mutable_snake()->set_id(snake->getId());

        snakes::position_msg_t* position = commonSnakeMsg.mutable_snake()->add_snakepart();
        position->set_x(snake->getHeadPosition().first);
        position->set_y(snake->getHeadPosition().second);
        sendMessageToClient(loginMessage->ownerConnection, commonSnakeMsg);
    }

    // Send food position to new player
    snakes::common_msg_t commonFoodMsg;
    commonFoodMsg.set_id(2);
    commonFoodMsg.mutable_food()->mutable_position()->set_x(_game.getFood()->getPosition().first);
    commonFoodMsg.mutable_food()->mutable_position()->set_y(_game.getFood()->getPosition().second);

    sendMessageToClient(loginMessage->ownerConnection, commonFoodMsg);
}

} // ns server
