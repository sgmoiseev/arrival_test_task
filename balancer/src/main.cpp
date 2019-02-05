#include "common.h"
#include "tcp-server/tcp-server.h"

#include <stdexcept>
#include <sstream>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>

boost::program_options::variables_map parse_command_line(int argc, const char* const *argv)
{
    namespace po = boost::program_options;
    po::variables_map vm;
    po::options_description desc{"Options"};

    try {
        desc.add_options()
        ("help,h", "Help message")
        ("route_map,r", po::value<std::string>()->default_value("./route-map.txt"), "File with route map");
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const po::required_option &ex) {
        if(vm.count("help")) {
            std::cout << desc << "\n" << std::endl;
        } else {
            throw ex;
        }
    }
    return vm;
}

balancer::route_map read_route_map(const std::string &file_path)
{
    balancer::route_map route_map;
    std::ifstream in_file{file_path};
    if(in_file) {
        std::cout << "Route map:" << std::endl;
        std::string line;
        while(std::getline(in_file, line)) {
            std::cout << line << std::endl;
            std::stringstream stream{line};
            std::uint32_t client_id; std::string host; std::uint16_t port;
            stream >> client_id >> host >> port;
            route_map.emplace(std::make_pair(client_id, common::remote_server{host, port}));
        }
    }
    return route_map;
}

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    try {
        const auto params{parse_command_line(argc, argv)};
        if(params.count("help")) {
            return 0;
        }

        const std::string route_map_file_path{params["route_map"].as<std::string>()};
        const auto route_map{read_route_map(route_map_file_path)};
        if(route_map.empty()) {
            throw std::invalid_argument{"Empty route map"};
        }

        balancer::tcp_server server{8888, route_map};
        server.start();
        server.stop();
    } catch (const std::exception &ex) {
        std::cout << "Server failed with error: " << ex.what() << std::endl;
    } catch (...) {
        std::cout << "Server failed with unknown error" << std::endl;
    }

    return 0;
}
