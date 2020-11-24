#include <iostream>

#include "server.h"

int main() {
    server::server_t server(9092);
    server.start();

    // NOTE: If removed server gets desctructed, but startMessageProcessingLoop does not return
    int serverCommand;
    std::cin >> serverCommand;

    return 0;
}
