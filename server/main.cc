#include <iostream>

#include "server.h"

int main() {
    server::server_t server(9092);
    server.start();
    return 0;
}
