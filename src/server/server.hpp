#include <iostream>
#include <chrono>
#include <memory>

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

#include "../exchange/exchange.hpp"
#include "handlers/order_handler.hpp"

class SeastarServer {

    public:
        SeastarServer(std::string name, std::string address, std::uint16_t port, std::shared_ptr<Exchange>& exchange);

        void set_routes();

        void start();

        void stop();

    private:

        void set_routes(seastar::httpd::routes& routes);

        seastar::httpd::match_rule* create_order_routes();

        std::string _address;
        std::uint16_t _port;
        OrderHandler _order_handler;
        std::unique_ptr<seastar::httpd::http_server> _server;
};
