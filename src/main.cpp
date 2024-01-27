#include <seastar/core/seastar.hh>
#include <seastar/core/future-util.hh>
#include <seastar/core/app-template.hh>
#include <iostream>
#include <gflags/gflags.h>

#include "utils/seastar_signal_catcher.hpp"

DEFINE_string(server_address, "0.0.0.0", "IP address on which server will listen.");
DEFINE_uint32(server_port, 8000, "Port on which the server will listen.");

int main(int argc, char** argv) {
    seastar::app_template app;

    app.add_options()("address", boost::program_options::value<std::string>()->default_value(FLAGS_server_address), "server address");
    app.add_options()("port", boost::program_options::value<u_int16_t>()->default_value(FLAGS_server_port), "server port");
    return app.run(argc, argv, [&] () -> seastar::future<int> {
        auto&& config = app.configuration();
        uint16_t port = config["port"].as<uint16_t>();
        std::string address = config["address"].as<std::string>();

        StopSignal stop_signal;
        co_await stop_signal.wait();
        co_return 0;
    });
}
