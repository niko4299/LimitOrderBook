#include <seastar/core/seastar.hh>
#include <seastar/core/future-util.hh>
#include <seastar/core/app-template.hh>
#include <seastar/http/routes.hh>
#include <seastar/http/request.hh>
#include <seastar/http/function_handlers.hh>
#include <seastar/http/httpd.hh>
#include <seastar/core/sleep.hh>
#include <seastar/coroutine/all.hh>
#include <seastar/util/log.hh>
#include <seastar/core/sharded.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/condition-variable.hh>
#include <iostream>
#include <chrono>
#include <gflags/gflags.h>


DEFINE_string(server_address, "0.0.0.0", "IP address on which server will listen.");
DEFINE_uint32(server_port, 8000, "Port on which the server will listen.");

using namespace std::chrono_literals;

seastar::logger applog{"http-requests"};

class stop_signal {
    public:
        stop_signal() {
            seastar::engine().handle_signal(SIGINT, [this] { signaled(); });
            seastar::engine().handle_signal(SIGTERM, [this] { signaled(); });
        }
        ~stop_signal() {
            seastar::engine().handle_signal(SIGINT, [] {});
            seastar::engine().handle_signal(SIGTERM, [] {});
        }
        seastar::future<> wait() {
            return _cond.wait([this] { return _caught; });
        }
        bool stopping() const {
            return _caught;
        }
    private:
        void signaled() {
            if (_caught) {
                return;
            }
            _caught = true;
            _cond.broadcast();
        }

    bool _caught = false;
    seastar::condition_variable _cond;
};

void set_routes(seastar::httpd::routes& r) {
    seastar::httpd::function_handler* h1 = new seastar::httpd::function_handler([](std::unique_ptr<seastar::http::request> req) -> seastar::future<seastar::json::json_return_type> {
        applog.info("{} slow", req->get_url());
        co_await seastar::sleep(10s);
        co_return seastar::json::json_return_type("json-future");
    });
    seastar::httpd::function_handler* h2 = new seastar::httpd::function_handler([](std::unique_ptr<seastar::http::request> req) -> seastar::future<seastar::json::json_return_type> {
        applog.info("{} fast", req->get_query_param("value"));
        
        co_return seastar::json::json_return_type("json-future");
    });
    r.add(seastar::httpd::operation_type::GET, seastar::httpd::url("/slow"), h1);
    r.add(seastar::httpd::operation_type::GET, seastar::httpd::url("/fast"), h2);
}

int main(int argc, char** argv) {
    seastar::app_template app;

    app.add_options()("address", boost::program_options::value<std::string>()->default_value(FLAGS_server_address), "server address");
    app.add_options()("port", boost::program_options::value<u_int16_t>()->default_value(FLAGS_server_port), "server port");
    return app.run(argc, argv, [&] () -> seastar::future<int> {
        auto&& config = app.configuration();
        uint16_t port = config["port"].as<uint16_t>();
        std::string address = config["address"].as<std::string>();

        stop_signal stop_signal;
        auto server = std::make_unique<seastar::httpd::http_server>("Unlimited OrderBook");
        set_routes(server->_routes);
        co_await server->listen(seastar::make_ipv4_address({address,port}));
        co_await stop_signal.wait();
        co_await server->stop();
        co_return 0;
    });
}
