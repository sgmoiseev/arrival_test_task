#include "tcp-server/tcp-server.h"

int main(int /*argc*/, char **/*argv */)
{
    try {
        balancer::tcp_server server{8888};
        server.start();
    } catch (const std::exception &ex ) {

    } catch (...) {

    }

    return 0;
}
