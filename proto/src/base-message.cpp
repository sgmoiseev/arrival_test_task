#include "base-message.h"

#include <arpa/inet.h>

namespace proto {

    const std::string base_message::message_prefix_{"msg#"};

    base_message::base_message(message_type type)
        : type_{type}
    { }

    base_message::base_message(const std::string &data)
        : message_data_{data.substr(message_prefix_.size())}
    { }

    void base_message::save()
    {
        message_stream_ << message_prefix_;
        save_uint32(static_cast<std::uint32_t>(type_));
    }

    void base_message::load()
    {
        type_ = static_cast<message_type>(load_uint32());
    }

    std::string base_message::as_string() const
    {
        return message_stream_.str();
    }

    message_type base_message::type() const noexcept
    {
        return type_;
    }

    std::size_t base_message::message_length() noexcept
    {
        return message_prefix_.size() + sizeof(message_type);
    }

    void base_message::save_uint32(std::uint32_t value)
    {
        const std::uint32_t be_value{htonl(value)};
        message_stream_ << std::string(reinterpret_cast<const char *>(&be_value),
                                       sizeof(be_value));
    }

    std::uint32_t base_message::load_uint32()
    {
        if(message_data_.size() < sizeof(std::uint32_t)) {
            throw std::runtime_error{"Can not read uint32: too little data"};
        }
        const std::uint32_t *value_ptr{reinterpret_cast<const std::uint32_t *>(message_data_.data())};
        const std::uint32_t value{ntohl(*value_ptr)};
        message_data_.erase(0, sizeof(std::uint32_t));
        return value;
    }

}
