#include <iostream>

#include "server.h"

int main() {
    server::server_t server(9092);
    server.start();

    int serverCommand;
    std::cin >> serverCommand;

    return 0;
}
