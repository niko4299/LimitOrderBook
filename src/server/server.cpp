#include "server.hpp"


SeastarServer::SeastarServer(std::string_view&& name, std::string_view&& address, std::uint16_t port, std::shared_ptr<Exchange>& exchange): _name{name}, _address{address}, _port{port}, _echange{exchange} {
    _server = std::make_unique<seastar::httpd::http_server>(name);
    set_routes(_server->_routes);
}

void SeastarServer::set_routes(seastar::httpd::routes& routes){
    seastar::httpd::function_handler* h1 = new seastar::httpd::function_handler([](std::unique_ptr<seastar::http::request> req) -> seastar::future<seastar::json::json_return_type> {
        applog.info("{} slow", req->get_url());
        co_await seastar::sleep(10s);
        co_return seastar::json::json_return_type("json-future");
    });

    auto create_new_order_route = create_new_order_route();
    routes.add(getConnectedById, seastar::httpd::operation_type::GET);
    
    seastar::httpd::function_handler* h2 = new seastar::httpd::function_handler([](std::unique_ptr<seastar::http::request> req) -> seastar::future<seastar::json::json_return_type> {
        applog.info("{} fast", req->get_query_param("value"));
        req->
        
        co_return seastar::json::json_return_type("json-future");
    });

    r.add(seastar::httpd::operation_type::GET, seastar::httpd::url("/slow"), h1);
    r.add(seastar::httpd::operation_type::GET, seastar::httpd::url("/fast"), h2);
}

std::shared_ptr<seastar::httpd::match_rule> SeastarServer::create_new_order_route(){
    auto function = [_exchange](std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> reply) -> seastar::future<std::unique_ptr<seastar::http::reply>>{
        auto& instrument = req->param["instrument"];
        req->content;
    };
    
    auto new_order_handler = std::shared_ptr<Handler>(std::forward(function));
    auto new_route = std::shared_ptr<seastar::httpd::match_rule>(new_order_handler);
    new_route->add_str("/orders/");
    new_route->add_param("instrument");
    new_route->add_str("/new");

    return new_route;
}


void SeastarServer::start(){
    co_await server->listen(seastar::make_ipv4_address({_address, _port}));
}


void SeastarServer::stop(){
    co_await server->stop();
}
