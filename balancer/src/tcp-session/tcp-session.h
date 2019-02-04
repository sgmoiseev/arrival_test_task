#pragma once

#include <logger/src/logger.h>
#include <proto/src/init-message.h>

#include <memory>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

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
        using bufferevent_ptr = std::unique_ptr<bufferevent, decltype(&bufferevent_free)>;

    public:
        tcp_session(event_base *base,
                    evutil_socket_t socket,
                    close_op_t close_op,
                    logger::logger &logger)
            : close_op_{close_op}
            , buffer_{bufferevent_ptr(bufferevent_socket_new(base, socket, BEV_OPT_CLOSE_ON_FREE), &bufferevent_free)}
            , logger_{logger}
        {}

        ~tcp_session() = default;

        void start() override
        {
            logger_.info("Start new unknown session");

            auto read_cd = [](bufferevent *buffer, void *ctx) {
                auto *self = static_cast<tcp_session*>(ctx);
                self->read_init_message(buffer);
            };
            auto *buff{buffer_.get()};
            bufferevent_setcb(buff, read_cd, nullptr, nullptr, this);
            bufferevent_setwatermark(buff, EV_READ, proto::init_message::message_length(), 0);
            bufferevent_enable(buff, EV_READ);
        }

        void stop() override
        {

        }

    private:
        void read_init_message(bufferevent *buffer)
        {
            evbuffer *buf_input = bufferevent_get_input(buffer);
            std::vector<std::uint8_t> data(proto::init_message::message_length());
            evbuffer_remove(buf_input, data.data(), data.size());
            proto::init_message init_msg{std::string{data.begin(), data.end()}};
            init_msg.load();
            logger_.info("We connected with client: ", init_msg.client_id());
        }

    private:
        close_op_t close_op_;
        bufferevent_ptr buffer_;
        logger::logger &logger_;
    };
}
