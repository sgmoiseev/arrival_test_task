#include "tcp-server/tcp-server.h"

#include <iostream>

int main(int /*argc*/, char **/*argv */)
{
    try {
        balancer::tcp_server server{8888};
        server.start();
    } catch (const std::exception &ex ) {
        std::cout << "Server failed with error: " << ex.what() << std::endl;
    } catch (...) {
        std::cout << "Server failed with unknown error" << std::endl;
    }

    return 0;
}
