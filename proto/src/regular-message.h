#pragma once

#include "base-message.h"

namespace proto {

    /*
     * init_message
     * |*****base_message*****|*****payload:uint32*****|
     * |                                               |
     * |----------------message_length-----------------|
     */

    class regular_message
        : public base_message
    {
    public:
        using payload_t = std::uint32_t;

    public:
        explicit regular_message(payload_t payload);
        explicit regular_message(const std::string &data);
        regular_message(regular_message &&rhs) = default;
        ~regular_message() override = default;

    public:
        payload_t payload() const noexcept;
        static std::size_t message_length() noexcept;

    public:
        void save();
        void load();

    private:
        payload_t payload_;
    };
}
