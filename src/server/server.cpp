#include "server.hpp"


SeastarServer::SeastarServer(std::string name, std::string address, std::uint16_t port, std::shared_ptr<Exchange>& exchange): _address{address}, _port{port}, _order_handler{exchange}, _orderbook_info_handler{exchange} {
    _server = std::make_unique<seastar::httpd::http_server_control>();
}

void SeastarServer::set_routes(seastar::httpd::routes& routes){
    routes.add(create_order_route(), seastar::httpd::operation_type::POST);
    routes.add(update_order_route(), seastar::httpd::operation_type::PUT);
    routes.add(get_order_route(), seastar::httpd::operation_type::GET);
    routes.add(cancel_order_route(), seastar::httpd::operation_type::PUT);
    routes.add(get_orderbook_snapshot_route(), seastar::httpd::operation_type::GET);
}

seastar::httpd::match_rule* SeastarServer::create_order_route(){
    auto new_route = new seastar::httpd::match_rule(&_order_handler._create_order_handler);
    new_route->add_str("/orders");
    new_route->add_param(INSTRUMENT_KEY);
    new_route->add_str("/create");
    
    return new_route;
}

seastar::httpd::match_rule* SeastarServer::update_order_route(){
    auto update_route = new seastar::httpd::match_rule(&_order_handler._update_order_handler);
    update_route->add_str("/orders");
    update_route->add_param(INSTRUMENT_KEY);
    update_route->add_str("/update");

    return update_route;
}

seastar::httpd::match_rule* SeastarServer::get_order_route(){
    auto get_route = new seastar::httpd::match_rule(&_order_handler._get_order_handler);
    get_route->add_str("/orders");
    get_route->add_param(INSTRUMENT_KEY);
    get_route->add_str("/");
    get_route->add_param(ORDER_ID_KEY);

    return get_route;
}

seastar::httpd::match_rule* SeastarServer::cancel_order_route(){
    auto cancel_route = new seastar::httpd::match_rule(&_order_handler._cancel_order_handler);
    cancel_route->add_str("/orders");
    cancel_route->add_param(INSTRUMENT_KEY);
    cancel_route->add_str("/cancel");
    cancel_route->add_param(ORDER_ID_KEY);

    return cancel_route;
}

seastar::httpd::match_rule* SeastarServer::get_orderbook_snapshot_route(){
    auto get_orderbook_snapshot_route = new seastar::httpd::match_rule(&_orderbook_info_handler._snapshot_handler);
    get_orderbook_snapshot_route->add_str("/orderbook");
    get_orderbook_snapshot_route->add_param(INSTRUMENT_KEY);
    get_orderbook_snapshot_route->add_str("/snapshot");
    
    return get_orderbook_snapshot_route;
}

void SeastarServer::start(){
    _server->start().get();   
    _server->set_routes([this](seastar::httpd::routes& r){set_routes(r);}).get();

    seastar::net::inet_address addr(_address);
    _server->listen(seastar::socket_address{addr, _port}).get();
}


seastar::future<> SeastarServer::stop(){
   return _server->stop();
}
