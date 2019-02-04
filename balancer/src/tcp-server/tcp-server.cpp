#include "tcp-server.h"
#include <common/src/utils.h>

namespace balancer {
    tcp_server::tcp_server(std::uint16_t port, const route_map &route_map)
        : port_{port}
        , route_map_{route_map}
    {}

    void tcp_server::start()
    {
        eb_ = common::event_base_ptr(event_base_new());
        check_null(eb_, "Can not create new event_base");

        const sockaddr_in sin{common::make_sockaddr(INADDR_ANY, port_)};

        const auto accept_conn_cb{
            [] (evconnlistener */*listener*/, evutil_socket_t socket, sockaddr *address, int /*socklen*/, void *ctx) {
                auto self{static_cast<tcp_server*>(ctx)};
                self->start_accept(socket, common::address_from_sockaddr(address));
            }
        };

        const auto listener_options{LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE};
        listener_ = common::listener_ptr(
                    evconnlistener_new_bind(eb_.get(), accept_conn_cb, this, listener_options,
                                            -1, reinterpret_cast<const sockaddr*>(&sin), sizeof(sin))
                    );
        check_null(listener_, "Can not create new listener");

        logger_.info("Start server");
        if(-1 == event_base_dispatch(eb_.get())) {
            throw std::runtime_error{"Can not run event loop"};
        }
    }

    void tcp_server::stop()
    {
        evconnlistener_disable(listener_.get());
        listener_.reset();

        for(const auto &session : sessions_) {
            session->stop();
        }
        event_base_loopbreak(eb_.get());
        eb_.reset();
    }

    void tcp_server::start_accept(evutil_socket_t socket, const std::string &client_addr)
    {
        logger_.info("New client connection was accepted, address: ", client_addr);
        const auto session_it{sessions_.emplace(sessions_.end())};
        auto close_op{[this, session_it]() { sessions_.erase(session_it); }};
        using session_t = tcp_session<decltype(close_op)>;
        *session_it = std::make_unique<session_t>(eb_.get(), socket, close_op, route_map_, logger_);
        (*session_it)->start();
    }
}
