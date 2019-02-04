#pragma once

#include <string>
#include <sstream>

namespace proto {

    enum class message_type : std::uint32_t {
        init = 0,
        regular
    };

    /*
     * base_message
     * |*****prefix*****|*****type*****|
     * |                               |
     * |--------message_length---------|
     */

    class base_message {
    public:
        explicit base_message(message_type type);
        explicit base_message(const std::string &data);
        base_message(base_message &&rhs) = default;
        virtual ~base_message() = default;

    public:
        message_type type() const noexcept;
        static std::size_t message_length() noexcept;

    public:
        void save();
        void load();
        std::string as_string() const;

    protected:
        void save_uint32(std::uint32_t value);
        std::uint32_t load_uint32();

    private:
        message_type type_;
        const static std::string message_prefix_;
        std::stringstream message_stream_;
        std::string message_data_;
    };

    template<typename msg_t,
             typename = std::enable_if<std::is_base_of<msg_t, proto::base_message>::value>
             >
    msg_t make_message(std::uint32_t value)
    {
        msg_t msg{value};
        msg.save();
        return msg;
    }

}
