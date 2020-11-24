#include "server.h"

#include <iostream>

using position_t = std::pair<uint32_t, uint32_t>;

namespace server {

server_t::server_t(const uint16_t port)
    : _net_server(std::make_unique<net::server::proto_server_t<snakes::common_msg_t>>(port)) {
    _game.createFood();
    _game.getTable()->debugPrint();
}

server_t::~server_t() {
    if(_messageProcessorThread.joinable())
        _messageProcessorThread.join();
};

void server_t::start() {
    _net_server->start();
    _messageProcessorThread = std::thread([this]() { processIncomingEvents(); });
}

void server_t::processIncomingEvents() {
    auto incomingMessagePtr = _net_server->getEvent();
    while(true) {
        if (incomingMessagePtr == nullptr) {
            std::cerr << "[GameServer] Incoming message is nullptr! Terminating." << std::endl;
        } else {
            switch (incomingMessagePtr->msg.id()) {
                case 1:
                    processLoginMessage(incomingMessagePtr);
                    break;
                default:
                    std::cerr << "[GameServer] Unknown event id" << std::endl;
                    break;
            }
        }
        incomingMessagePtr = _net_server->getEvent();
    }
}

// TODO: When client disconnects, connections is ereased, but the snake stays on the table
// with each restart of the client sent eventcounter grows ...
void server_t::processLoginMessage(std::shared_ptr<net::common::owned_message_t<snakes::common_msg_t>> loginMessage) {
    std::cout << "[GameServer] Processing login message " << std::endl;
    position_t playerPosition = _game.placePlayerOnTable(loginMessage->ownerConnection->getId(), loginMessage->msg.login().username());

    // Broadcast new player position
    snakes::common_msg_t commonSnakeMsg;
    commonSnakeMsg.set_id(3);
    commonSnakeMsg.mutable_snake()->set_id(loginMessage->ownerConnection->getId());

    snakes::position_msg_t* position = commonSnakeMsg.mutable_snake()->add_snakepart();
    position->set_x(playerPosition.first);
    position->set_y(playerPosition.second);

    _net_server->broadcast(commonSnakeMsg);

    // Send already added snakes to new player
    // TODO: Sends duplicate of the newly added player
    for (const auto& snake : _game.getSnakes()) {
        snakes::common_msg_t commonSnakeMsg;
        commonSnakeMsg.set_id(3);
        commonSnakeMsg.mutable_snake()->set_id(snake->getId());

        snakes::position_msg_t* position = commonSnakeMsg.mutable_snake()->add_snakepart();
        position->set_x(snake->getHeadPosition().first);
        position->set_y(snake->getHeadPosition().second);
        loginMessage->ownerConnection->send(commonSnakeMsg);
    }

    // Send food position to new player
    snakes::common_msg_t commonFoodMsg;
    commonFoodMsg.set_id(2);
    commonFoodMsg.mutable_food()->mutable_position()->set_x(_game.getFood()->getPosition().first);
    commonFoodMsg.mutable_food()->mutable_position()->set_y(_game.getFood()->getPosition().second);

    loginMessage->ownerConnection->send(commonFoodMsg);

    // Clear received message pointer
    loginMessage.reset();
}

} // ns server
