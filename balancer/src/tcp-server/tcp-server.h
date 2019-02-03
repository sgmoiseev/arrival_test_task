#pragma once

#include <logger/src/logger.h>
#include "../tcp-session/tcp-session.h"

#include <list>
#include <memory>
#include <event2/listener.h>

namespace balancer {

    class tcp_server{
        using event_base_ptr = std::unique_ptr<event_base, decltype(&event_base_free)>;
        using listener_ptr = std::unique_ptr<evconnlistener, decltype(&evconnlistener_free)>;

    public:
        explicit tcp_server(std::uint16_t port);
        void start();

    private:
        void start_accept(evutil_socket_t socket, const std::string &client_addr);

        template<typename T>
        void check_null(const T &ptr, const std::string &error_msg)
        {
            if(!ptr) {
                logger_.error(error_msg);
                throw std::runtime_error{error_msg};
            }
        }

    private:
        const std::uint16_t port_;
        event_base_ptr eb_{nullptr, nullptr};
        listener_ptr listener_{nullptr, nullptr};
        logger::logger logger_{"tcp_server"};
        std::list<std::unique_ptr<session_iface>> sessions_;
    };

}
