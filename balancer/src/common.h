#pragma once

#include <common/src/remote-server.h>
#include <proto/src/init-message.h>

#include <map>

namespace balancer {

    using route_map = std::map<proto::init_message::client_id_t, common::remote_server>;

}
