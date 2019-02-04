#pragma once

#include "utils.h"

#include <iostream>

namespace common {

    class remote_server {
    public:
        remote_server(const std::string &host, std::uint16_t port);
        sockaddr_in sockaddr() const;
        friend std::ostream &operator<<(std::ostream &os, const remote_server &rs);

    private:
        bool is_ipv4() const noexcept;
        std::string resolve_host_by_name() const;

    private:
        const std::string host_;
        const std::uint16_t port_;
    };

    std::ostream& operator<<(std::ostream& os, const remote_server &rs);

}
