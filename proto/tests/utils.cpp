#include "utils.h"

void foreach_message_type(const std::function<void(message_type type)> &func)
{
    for(const auto type : {message_type::init, message_type::regular}) {
        func(type);
    }
}
