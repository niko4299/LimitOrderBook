#include <iostream>
#include <chrono>

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
#include <seastar/apps/lib/stop_signal.hh>

#include "../exchange/exchange.hpp"

final class SeastarServer{

    public:
        SeastarServer(std::string_view&& name, std::string_view&& address, std::uint16_t port, std::shared_ptr<Exchange>& exchange);

        void set_routes(seastar::httpd::routes& routes);

        void start();

        void stop();
    
    private:

        std::shared_ptr<seastar::httpd::match_rule> create_new_order_route();


        std::unique_ptr<seastar::httpd::http_server> _server;
        std::string_view _name;
        std::string_view _address;
        std::uint16_t _port;
        std::shared_ptr<Exchange>& exchange;
}
