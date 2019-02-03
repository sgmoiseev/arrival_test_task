#include "utils.h"
#include "../src/base-message.h"

#include <arpa/inet.h>
#include <gtest/gtest.h>

using namespace proto;

const std::string prefix{"msg#"};

TEST(base_message, BaseMessagePrefix)
{
    // string prefix size + sizeof(message_type)
    ASSERT_EQ(prefix.size() +sizeof(message_type),
              base_message::message_length());
}

TEST(base_message, CheckMessageType)
{
    foreach_message_type([](message_type type){
        base_message msg{type};
        EXPECT_EQ(type, msg.type());
    });
}

TEST(base_message, SaveToStream)
{
    foreach_message_type([](message_type type){
        base_message msg{type};
        msg.save();
        const std::uint32_t type_value{htonl(static_cast<std::uint32_t>(type))};
        const std::string  type_str{reinterpret_cast<const char *>(&type_value), sizeof (type_value)};
        EXPECT_EQ(prefix + type_str, msg.as_string());
    });
}

TEST(base_message, LoadFromStream)
{
    foreach_message_type([](message_type type){
        base_message msg{type};
        msg.save();
        std::string tmp{msg.as_string()};
        base_message new_msg{msg.as_string()};
        new_msg.load();
        EXPECT_EQ(type, new_msg.type());
    });
}
