#include "../src/regular-message.h"

#include <gtest/gtest.h>

using namespace proto;

TEST(regular_message, RegularMessagePrefix)
{
    // base_message prefix size + sizeof(payload_t)
    ASSERT_EQ(base_message::message_length() + sizeof(regular_message::payload_t),
              regular_message::message_length());
}

TEST(regular_message, SaveToAndLoadFromStream)
{
    regular_message msg{1024};
    msg.save();
    regular_message new_msg{msg.as_string()};
    new_msg.load();
    ASSERT_EQ(msg.payload(), new_msg.payload());
}


