#include "server.h"

#include <chrono>
#include <iostream>

using position_t = std::pair<uint32_t, uint32_t>;

namespace server {

server_t::server_t(const uint16_t port) : net::server::proto_server_t<snakes::common_msg_t>(port) {
    _game.createFood();
}

void server_t::startGame() {
    std::cout << "[GameServer] Starting game!" << std::endl;

    while (!_game.isGameOver()) {
        // Step game
        _game.step();

        // Broadcast modified fields
        snakes::common_msg_t modifiedFieldsMsg;
        modifiedFieldsMsg.set_id(2);

        // Add food field (is it did not change, this is an overhead TODO)
        // BUG: Reprints the food on the snakes position after consumption
        snakes::position_msg_t* foodPosition = modifiedFieldsMsg.mutable_field_change()->add_position();
        foodPosition->set_field_type(snakes::field_t::FOOD);
        foodPosition->set_x(_game.getFood()->getPosition().first);
        foodPosition->set_y(_game.getFood()->getPosition().second);

        for (const auto& snake : _game.getSnakes()) {
            if (!snake->isAlive())
                // TODO: Send empty for each position of the snake to clear its place on the table
                continue;

            // Add head position of alive snake
            snakes::position_msg_t* snakeHeadPosition = modifiedFieldsMsg.mutable_field_change()->add_position();
            snakeHeadPosition->set_field_type(snakes::field_t::SNAKE);
            snakeHeadPosition->set_id(snake->getId());
            snakeHeadPosition->set_x(snake->getHeadPosition().first);
            snakeHeadPosition->set_y(snake->getHeadPosition().second);

            // Clear trail position of alive snake
            snakes::position_msg_t* snakeTrailPosition = modifiedFieldsMsg.mutable_field_change()->add_position();
            snakeTrailPosition->set_field_type(snakes::field_t::EMPTY);
            snakeTrailPosition->set_x(snake->getTrailPosition().first);
            snakeTrailPosition->set_y(snake->getTrailPosition().second);
        }

        broadcastMessage(modifiedFieldsMsg);

        // Broadcast game stepped message
        snakes::common_msg_t stepGameMsg;
        stepGameMsg.set_id(3);
        broadcastMessage(stepGameMsg);

        _game.getTable()->debugPrint();

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
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
        case 4:
            changeSnakeDirection(message);
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

        snakes::position_msg_t* snakePosition = snakeMsg.mutable_field_change()->add_position();
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

void server_t::changeSnakeDirection(const std::shared_ptr<const net::common::owned_message_t<snakes::common_msg_t>> dirChangeMessage) {
    std::cout << "[GameServer] Processing direction change message " << std::endl;

    for (const auto& snake : _game.getSnakes()) {
        if (snake->getId() == dirChangeMessage->ownerConnection->getId()) {
            switch (dirChangeMessage->msg.change_dir().new_direction()) {
                case snakes::direction_t::LEFT:
                    snake->changeDirection(common::game_model::direction_e::LEFT);
                    break;
                case snakes::direction_t::UP:
                    snake->changeDirection(common::game_model::direction_e::UP);
                    break;
                case snakes::direction_t::DOWN:
                    snake->changeDirection(common::game_model::direction_e::DOWN);
                    break;
                case snakes::direction_t::RIGHT:
                    snake->changeDirection(common::game_model::direction_e::RIGHT);
                    break;
            }
            return;
        }
    }
}

} // ns server
