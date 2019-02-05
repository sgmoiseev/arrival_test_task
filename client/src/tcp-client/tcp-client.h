#pragma once

#include <common/src/remote-server.h>
#include <logger/src/logger.h>
#include <proto/src/base-message.h>

struct bufferevent;

namespace tcp_client {

    class tcp_client {
    public:
        tcp_client(std::uint32_t client_id, const std::string &host,
                   std::uint16_t port, std::uint32_t max_msg_count);
        void start();

    private:
        void write_message(bufferevent *bev, const proto::bytes &msg);
        void on_ready_write(bufferevent *bev);
        void on_next_event(short what);

        template<typename ptr_t>
        void check_null(const ptr_t &ptr, const std::string &error_msg)
        {
            if(!ptr) {
                log_error_and_throw(error_msg);
            }
        }

        void check_result_code(int result_code, const std::string &error_msg);
        void log_error_and_throw(const std::string &error_msg);

    private:
        logger::logger logger_;
        const std::uint32_t client_id_;
        const common::remote_server r_server_;
        const std::uint32_t max_msg_count_;
        std::uint32_t curr_msg_number_{0};
    };

}
