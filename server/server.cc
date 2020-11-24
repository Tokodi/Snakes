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
    snakes::common_msg_t newSnakeMsg;
    newSnakeMsg.set_id(2);

    snakes::position_msg_t* newSnakePosition = newSnakeMsg.mutable_field_change()->add_position();
    newSnakePosition->set_field_type(snakes::field_t::SNAKE);
    newSnakePosition->set_id(loginMessage->ownerConnection->getId());
    newSnakePosition->set_x(playerPosition.first);
    newSnakePosition->set_y(playerPosition.second);

    broadcastMessage(newSnakeMsg, loginMessage->ownerConnection);

    // TODO: Send the whole snake, not just the head, for later joiners
    // Send already online players position to newcomer
    for (const auto& snake : _game.getSnakes()) {
        snakes::common_msg_t snakeMsg;
        snakeMsg.set_id(2);

        snakes::position_msg_t* snakePosition = newSnakeMsg.mutable_field_change()->add_position();
        snakePosition->set_field_type(snakes::field_t::SNAKE);
        snakePosition->set_id(snake->getId());
        snakePosition->set_x(snake->getHeadPosition().first);
        snakePosition->set_y(snake->getHeadPosition().second);

        sendMessageToClient(loginMessage->ownerConnection, snakeMsg);
    }

    // Send food position to new player
    snakes::common_msg_t foodMsg;
    foodMsg.set_id(2);

    snakes::position_msg_t* foodPosition = foodMsg.mutable_field_change()->add_position();
    foodPosition->set_field_type(snakes::field_t::FOOD);
    foodPosition->set_x(_game.getFood()->getPosition().first);
    foodPosition->set_y(_game.getFood()->getPosition().second);

    sendMessageToClient(loginMessage->ownerConnection, foodMsg);
}

} // ns server
