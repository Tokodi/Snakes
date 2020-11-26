#include <iostream>

#include "client.h"
#include "ui/utils.h"

int main() {
    //std::string hostIp = "192.168.1.160";
    std::string hostIp = "127.0.0.1";
    uint16_t port = 9092;

//    std::cout << "Host ip: ";
//    std::cin >> hostIp;
//    std::cout << "Port: ";
//    std::cin >> port;

    client::client_t client(hostIp, port);

    std::string username = "Andris";
//    std::cout << "Username: ";
//    std::cin >> username;

    client.login(username);

//    client::ui::initialize();
//
//    client.show_ui();

    // NOTE: If removed client gets desctructed, but startMessageProcessingLoop does not return
//    int clientCommand;
//    std::cin >> clientCommand;
//

    client.listenForKeyboardInput();

    return 0;
}
