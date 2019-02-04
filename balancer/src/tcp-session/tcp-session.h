#pragma once

#include <common/src/types.h>
#include <logger/src/logger.h>
#include <proto/src/init-message.h>

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
                    logger::logger &logger)
            : close_op_{close_op}
            , buffer_{common::bufferevent_ptr(bufferevent_socket_new(base, socket, BEV_OPT_CLOSE_ON_FREE))}
            , logger_{logger}
        {}

        ~tcp_session() = default;

        void start() override
        {
            logger_.info("Start new unknown session");

            const auto read_cd = [](bufferevent */*buffer*/, void *ctx) {
                auto *self{static_cast<tcp_session *>(ctx)};
                self->read_init_message();
            };

            const auto on_event = [](bufferevent */*bev*/, short what, void *ctx)
            {
                auto *self{static_cast<tcp_session *>(ctx)};
                self->on_next_event(what);
            };

            auto *buff{buffer_.get()};
            bufferevent_setcb(buff, read_cd, nullptr, on_event, this);
            bufferevent_setwatermark(buff, EV_READ, proto::init_message::message_length(), 0);
            bufferevent_enable(buff, EV_READ);
        }

        void stop() override
        {
            bufferevent_disable(buffer_.get(), EV_READ);
            buffer_.reset();
            close_op_();
        }

    private:
        void read_init_message()
        {
            auto *buf_input{bufferevent_get_input(buffer_.get())};
            std::vector<std::uint8_t> data(proto::init_message::message_length());
            evbuffer_remove(buf_input, data.data(), data.size());
            proto::init_message init_msg{std::string{data.begin(), data.end()}};
            init_msg.load();
            logger_.info("We connected with client: ", init_msg.client_id());
        }

        void on_next_event(short what)
        {
            if(what & BEV_EVENT_EOF) {
                logger_.info("Client was disconnected");
            }
            if(what & BEV_EVENT_ERROR) {
                logger_.error("An error occurred during a bufferevent operation");
            }
            logger_.info("Close session");
            stop();
        }

    private:
        close_op_t close_op_;
        common::bufferevent_ptr buffer_;
        logger::logger &logger_;
    };
}
