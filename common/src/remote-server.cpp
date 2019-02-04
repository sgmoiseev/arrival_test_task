#include "remote-server.h"

#include <stdexcept>
#include <netdb.h>

namespace common {

    remote_server::remote_server(const std::string &host, std::uint16_t port)
        : host_{host}
        , port_{port}
    { }

    sockaddr_in remote_server::sockaddr() const
    {
        if(is_ipv4()) {
            return make_sockaddr(host_, port_);
        }
        return make_sockaddr(resolve_host_by_name(), port_);
    }

    bool remote_server::is_ipv4() const noexcept
    {
        sockaddr_in sa;
        return 0 != inet_pton(AF_INET, host_.c_str(), &(sa.sin_addr));
    }

    std::string remote_server::resolve_host_by_name() const
    {
        hostent *he{gethostbyname(host_.c_str())};
        if (nullptr == he) {
            throw std::invalid_argument{"Can not get host by name: " + host_};
        }
        auto addr_list{reinterpret_cast<in_addr **>(he->h_addr_list)};
        return inet_ntoa(*addr_list[0]);
    }

    std::ostream& operator<<(std::ostream& os, const remote_server &rs)
    {
        os << rs.host_ << ":" << rs.port_;
        return os;
    }

}
