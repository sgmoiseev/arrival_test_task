#include "common.h"
#include "tcp-server/tcp-server.h"

#include <iostream>

int main(int /*argc*/, char **/*argv */)
{
    try {
        balancer::route_map map{
            {1, common::remote_server{"n1.example.com", 7777}},
            {2, common::remote_server{"n2.example.com", 9999}},
            {3, common::remote_server{"n2.example.com", 9999}}
        };
        balancer::tcp_server server{8888, map};
        server.start();
    } catch (const std::exception &ex ) {
        std::cout << "Server failed with error: " << ex.what() << std::endl;
    } catch (...) {
        std::cout << "Server failed with unknown error" << std::endl;
    }

    return 0;
}
