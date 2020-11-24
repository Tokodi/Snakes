#include "client.h"

#include <iostream>

namespace client {

client_t::client_t(const std::string& host, const uint16_t port)
    : _net_client(std::make_unique<net::client::proto_client_t<snakes::common_msg_t>>()) {
    _net_client->connect(host, port);
    _messageProcessorThread = std::thread([this]() { processIncomingEvents(); });
    _view = std::make_shared<ui::ncurses_view>(60, 60); // TODO: Remove size constant...
}

client_t::~client_t() {
    if(_messageProcessorThread.joinable())
        _messageProcessorThread.join();
};

void client_t::processIncomingEvents() {
    auto incomingMessagePtr = _net_client->getEvent();
    while(1) {
        if (incomingMessagePtr == nullptr) {
            std::cerr << "[GameClient] Received nullptr!" << std::endl;
        } else {
            switch (incomingMessagePtr->msg.id()) {
                case 2:
                    std::cout << "("
                              << incomingMessagePtr->msg.food().position().x()
                              << ", "
                              << incomingMessagePtr->msg.food().position().y()
                              << ")"
                              << std::endl;
                    _game.drawFoodOnTable(std::pair<uint32_t, uint32_t>(
                                incomingMessagePtr->msg.food().position().x(),
                                incomingMessagePtr->msg.food().position().y())
                    );
                    _game.getTable()->debugPrint();
                    break;
                case 3:
                    //std::cout << "[GameClient] Process Snake message" << std::endl;

                    break;
                case 4:
                    //std::cout << "[GameClient] Process Game Step message" << std::endl;
                    _view->draw(_game.getTable());
                    break;
                default:
                    std::cout << "[GameClient] Unknown event id" << std::endl;
            }
        }
        incomingMessagePtr = _net_client->getEvent();
    }
}

void client_t::login(const std::string& userName) {
    snakes::common_msg_t loginMsg;
    loginMsg.set_id(1);
    _net_client->sendMessage(loginMsg);
}

void client_t::show_ui() {
    _view->show();
}

} // ns client
