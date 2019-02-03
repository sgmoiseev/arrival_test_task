#include "../src/init-message.h"

#include <gtest/gtest.h>

using namespace proto;

TEST(init_message, InitMessagePrefix)
{
    // base_message prefix size + sizeof(client_id_t)
    ASSERT_EQ(base_message::message_length() + sizeof(init_message::client_id_t),
              init_message::message_length());
}

TEST(init_message, SaveToAndLoadFromStream)
{
    init_message msg{1024};
    msg.save();
    init_message new_msg{msg.as_string()};
    new_msg.load();
    ASSERT_EQ(msg.client_id(), new_msg.client_id());
}


