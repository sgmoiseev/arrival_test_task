#include "../src/base-message.h"

#include <functional>

using namespace proto;

void foreach_message_type(const std::function<void(message_type type)> &func);
