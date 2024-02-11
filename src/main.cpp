#include <seastar/core/seastar.hh>
#include <seastar/core/future-util.hh>
#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/thread.hh>
#include <iostream>
#include <gflags/gflags.h>

#include "server/server.hpp"
#include "utils/seastar_signal_catcher.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>

std::vector<std::pair<std::string, float>> read_instrument_init_data_from_file(const std::string& file_path) {
    std::vector<std::pair<std::string, float>> instruments_info;

    std::ifstream input_file(file_path);

    if (!input_file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return instruments_info;
    }

    std::string line;
    while (std::getline(input_file, line)) {
        std::istringstream iss(line);

        std::string symbol;
        float value;

        if (iss >> symbol >> value) {
            instruments_info.push_back(std::make_pair(symbol, value));
        } else {
            std::cerr << "Error parsing line: " << line << std::endl;
        }
    }

    input_file.close();

    return instruments_info;
}

DEFINE_string(server_name, "UnlimitedExchange", "server name.");
DEFINE_string(server_address, "0.0.0.0", "ip address on which server will listen.");
DEFINE_uint32(server_port, 8000, "port on which the server will listen.");
DEFINE_string(trade_repository_address, "0.0.0.0", "ip address of scylla database.");
DEFINE_uint32(trade_repository_batch_size, 1, "batch size for inserting into trade repository (currently not supported so value should be set to 1).");
DEFINE_string(rocksdb_dir, "./order_rocksdb", "order rocksdb directory.");
DEFINE_uint32(ringbuffer_size, 1024, "ringbuffer size (currently same for all).");
DEFINE_string(instrument_init_file, "../init/instruments.txt", "initial insturment info.");


int main(int argc, char** argv) {
    seastar::app_template app;

    app.add_options()("server_name", boost::program_options::value<std::string>()->default_value(FLAGS_server_name), "server name");
    app.add_options()("server_address", boost::program_options::value<std::string>()->default_value(FLAGS_server_address), "server address");
    app.add_options()("server_port", boost::program_options::value<u_int16_t>()->default_value(FLAGS_server_port), "server port");
    app.add_options()("trade_repository_address", boost::program_options::value<std::string>()->default_value(FLAGS_trade_repository_address), "trade repository address");
    app.add_options()("trade_repository_batch_size", boost::program_options::value<uint32_t>()->default_value(FLAGS_trade_repository_batch_size), "trade repository batch size");
    app.add_options()("rocksdb_dir", boost::program_options::value<std::string>()->default_value(FLAGS_rocksdb_dir), "rocksdb directory");
    app.add_options()("ringbuffer_size", boost::program_options::value<uint32_t>()->default_value(FLAGS_ringbuffer_size), "ringbuffer size");
    app.add_options()("instrument_init_file", boost::program_options::value<std::string>()->default_value(FLAGS_instrument_init_file), "instrument init file");
        
    return app.run(argc, argv, [&] () -> seastar::future<int> {
       
       return seastar::async([&] {
        auto&& config = app.configuration();
        
        std::string server_name = config["server_name"].as<std::string>();
        std::string address = config["server_address"].as<std::string>();
        uint16_t port = config["server_port"].as<uint16_t>();
        std::string trade_repository_address = config["trade_repository_address"].as<std::string>();
        uint32_t trade_repository_batch_size = config["trade_repository_batch_size"].as<uint32_t>();
        std::string rocks_db_dir = config["rocksdb_dir"].as<std::string>();
        uint32_t ring_buffer_size = config["ringbuffer_size"].as<uint32_t>();
        std::string instrument_init_file = config["instrument_init_file"].as<std::string>();

        StopSignal stop_signal;
        auto _order_repository = std::make_shared<OrderRepository>(std::move(rocks_db_dir), ring_buffer_size);
        auto _trade_repository = std::make_shared<TradeRepository>(std::move(trade_repository_address), trade_repository_batch_size, ring_buffer_size);
        auto instruments_info = read_instrument_init_data_from_file(instrument_init_file);
        auto exchange = std::make_shared<Exchange>(instruments_info, ring_buffer_size, _order_repository, _trade_repository);

        SeastarServer server{server_name, address, port, exchange};
        server.start();

        stop_signal.wait().get();
        server.stop().get();
        return 0;
       });
    });
}
