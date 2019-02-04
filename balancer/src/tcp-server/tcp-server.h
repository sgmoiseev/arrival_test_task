#pragma once

#include "../common.h"
#include <common/src/types.h>
#include <logger/src/logger.h>
#include "../tcp-session/tcp-session.h"

#include <list>

namespace balancer {

    class tcp_server{

    public:
        tcp_server(std::uint16_t port, const route_map &route_map);
        void start();

    private:
        void start_accept(evutil_socket_t socket, const std::string &client_addr);

        template<typename ptr_t>
        void check_null(const ptr_t &ptr, const std::string &error_msg)
        {
            if(!ptr) {
                logger_.error(error_msg);
                throw std::runtime_error{error_msg};
            }
        }

    private:
        const std::uint16_t port_;
        const route_map route_map_;
        common::event_base_ptr eb_{nullptr};
        common::listener_ptr listener_{nullptr};
        logger::logger logger_{"tcp_server"};
        std::list<std::unique_ptr<session_iface>> sessions_;
    };

}
