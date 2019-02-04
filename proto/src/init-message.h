#pragma once

#include "base-message.h"

namespace proto {

    /*
     * init_message
     * |*****base_message*****|*****client_id:uint32*****|
     * |                                                 |
     * |-----------------message_length------------------|
     */

    class init_message
        : public base_message
    {
    public:
        using client_id_t = std::uint32_t;

    public:
        explicit init_message(client_id_t clietn_id);
        explicit init_message(const std::string &data);
        init_message(init_message &&rhs) = default;
        ~init_message() override = default;

    public:
        client_id_t client_id() const noexcept;
        static std::size_t message_length() noexcept;

    public:
        virtual void save();
        virtual void load();

    private:
        client_id_t client_id_;
    };

    proto::init_message make_init_message(proto::init_message::client_id_t client_id);

}
