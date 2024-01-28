#include "server.hpp"


SeastarServer::SeastarServer(std::string name, std::string address, std::uint16_t port, std::shared_ptr<Exchange>& exchange): _address{address}, _port{port}, _order_handler(exchange) {
    _server = std::make_unique<seastar::httpd::http_server>(name);
}


void  SeastarServer::set_routes(){
    set_routes(_server->_routes);
}

void SeastarServer::set_routes(seastar::httpd::routes& routes){
    seastar::httpd::function_handler* h1 = new seastar::httpd::function_handler([](std::unique_ptr<seastar::http::request> req) -> seastar::future<seastar::json::json_return_type> {
        // applog.info("{} slow", req->get_url());
        co_return seastar::json::json_return_type("json-future");
    });

    auto new_order_route = create_order_routes();
    routes.add(new_order_route, seastar::httpd::operation_type::GET);
    
    seastar::httpd::function_handler* h2 = new seastar::httpd::function_handler([](std::unique_ptr<seastar::http::request> req) -> seastar::future<seastar::json::json_return_type> {
        // applog.info("{} fast", req->get_query_param("value"));
        // // req->
        
        co_return seastar::json::json_return_type("json-future");
    });

    routes.add(seastar::httpd::operation_type::GET, seastar::httpd::url("/slow"), h1);
    routes.add(seastar::httpd::operation_type::GET, seastar::httpd::url("/fast"), h2);
}

seastar::httpd::match_rule* SeastarServer::create_order_routes(){
    
    auto new_route = new seastar::httpd::match_rule(&_order_handler._get_order_handler);
    new_route->add_str("/orders/");
    new_route->add_param("instrument");
    new_route->add_str("/new");

    return new_route;
}


void SeastarServer::start(){
   auto error_code = _server->listen(seastar::make_ipv4_address({_address, _port}));
}


void SeastarServer::stop(){
    auto error_code = _server->stop();
}
