#pragma once

#include "../common.h"
#include <common/src/types.h>
#include <logger/src/logger.h>
#include <proto/src/init-message.h>
#include <proto/src/regular-message.h>

namespace balancer {

    class session_iface {
    public:
        virtual ~session_iface() = default;
        virtual void start() = 0;
        virtual void stop() = 0;
    };

    template<typename close_op_t>
    class tcp_session
        : public session_iface
    {

    public:
        tcp_session(event_base *base,
                    evutil_socket_t socket,
                    close_op_t close_op,
                    const route_map &route_map,
                    logger::logger &logger)
            : close_op_{close_op}
            , route_map_{route_map}
            , client_buffer_{common::bufferevent_ptr(bufferevent_socket_new(base, socket, BEV_OPT_CLOSE_ON_FREE))}
            , logger_{logger}
        {}

        ~tcp_session() = default;

        void start() override
        {
            if(!client_buffer_) {
                logger_.error("Invalid bufferevent");
                close_op_();
                return;
            }

            logger_.info("Start new unknown session");
            start_reading_init_message();
        }

        void stop() override
        {
            if(client_buffer_) {
                bufferevent_disable(client_buffer_.get(), EV_READ);
                client_buffer_.reset();
            }
            if(server_buffer_) {
                bufferevent_disable(server_buffer_.get(), EV_WRITE);
                server_buffer_.reset();
            }
            close_op_();
        }

    private:
        void start_reading_init_message()
        {
            const auto read_cd = [](bufferevent */*buffer*/, void *ctx) {
                auto *self{static_cast<tcp_session *>(ctx)};
                self->read_init_message();
            };
            prepare_client_buffer_for_reading(read_cd, proto::init_message::message_length(), 0);
        }

        void read_init_message()
        {
            const auto msg{read_message<proto::init_message>()};
            logger_.info("We connected with client: ", msg.client_id());
            start_routing(msg.client_id());
        }

        void start_routing(proto::init_message::client_id_t client_id)
        {
            const auto server_it = route_map_.find(client_id);
            if(route_map_.cend() == server_it) {
                logger_.error("Have no information about client: ", client_id);
                stop();
            } else {
                auto &srv{server_it->second};
                connect_to_server(srv);
                logger_.info("Start routing packets from clietn ", client_id, " to server ", srv);
                start_reading_regular_message();
            }
        }

        void connect_to_server(const common::remote_server &server)
        {
            server_buffer_ = common::bufferevent_ptr(bufferevent_socket_new(bufferevent_get_base(client_buffer_.get()),
                                                                            -1, BEV_OPT_CLOSE_ON_FREE));
            if(!server_buffer_) {
                logger_.error("Can not connect to server");
                stop();
            }

            auto *buff{server_buffer_.get()};
            bufferevent_setcb(buff, nullptr, nullptr, tcp_session::on_event_cb, this);
            bufferevent_enable(buff, EV_WRITE);
            bufferevent_disable(buff, EV_READ);

            const auto sock{server.sockaddr()};
            bufferevent_socket_connect(buff, reinterpret_cast<const sockaddr *>(&sock), sizeof(sock));
        }

        void start_reading_regular_message()
        {
            const auto read_cd = [](bufferevent */*buffer*/, void *ctx) {
                auto *self{static_cast<tcp_session *>(ctx)};
                self->read_regular_message();
            };

            prepare_client_buffer_for_reading(read_cd, proto::regular_message::message_length(), 0);
        }

        void read_regular_message()
        {
            const auto msg{read_message<proto::regular_message>()};
            logger_.info("Message with payload '", msg.payload() ,"' has been received");
            write_regular_message(msg.row_data());
        }

        void write_regular_message(const std::string &msg)
        {
            bufferevent_write(server_buffer_.get(), msg.data(), msg.size());
        }

        void on_next_event(short what)
        {
            if(what & BEV_EVENT_EOF || what & BEV_EVENT_ERROR) {
                logger_.info("Close session");
                stop();
            }
        }

        template<typename r_cb_t>
        void prepare_client_buffer_for_reading(const r_cb_t &r_cb, std::size_t lowmark, std::size_t highmark)
        {
            auto *buff{client_buffer_.get()};
            bufferevent_setcb(buff, r_cb, nullptr, tcp_session::on_event_cb, this);
            bufferevent_setwatermark(buff, EV_READ, lowmark, highmark);
            bufferevent_enable(buff, EV_READ);
            bufferevent_disable(buff, EV_WRITE);
        }

        template<typename msg_t>
        msg_t read_message()
        {
            std::vector<std::uint8_t> data(msg_t::message_length());
            bufferevent_read(client_buffer_.get(), data.data(), data.size());
            msg_t msg{std::string(data.cbegin(), data.cend())};
            msg.load();
            return msg;
        }

        static void on_event_cb(bufferevent */*bev*/, short what, void *ctx)
        {
            auto *self{static_cast<tcp_session *>(ctx)};
            self->on_next_event(what);
        }

    private:
        close_op_t close_op_;
        const route_map &route_map_;
        common::bufferevent_ptr client_buffer_;
        common::bufferevent_ptr server_buffer_;
        logger::logger &logger_;
    };
}
