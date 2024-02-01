#include "order_handler.hpp"

OrderHandler::OrderHandler(std::shared_ptr<Exchange>& exchange): _new_order_handler(*this), _update_order_handler(*this), _get_order_handler(*this), _exchange{exchange} {}


OrderHandler::NewOrderHandler::NewOrderHandler(OrderHandler& parent): _parent{parent} {}


seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::NewOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        simdjson::ondemand::parser parser;
        simdjson::padded_string json(req->content.c_str(),req->content.size()); 
        simdjson::ondemand::document doc = parser.iterate(json);
        double x = doc["x"];
        rep->write_body("json", seastar::json::stream_object(x));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}

OrderHandler::GetOrderHandler::GetOrderHandler(OrderHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::GetOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        rep->write_body("json", seastar::json::stream_object("TEST"));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}

OrderHandler::UpdateOrderHandler::UpdateOrderHandler(OrderHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::UpdateOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        rep->write_body("json", seastar::json::stream_object("TEST"));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}

// NewOrderHandler& OrderHandler::get_new_order_handler(){
//         return _new_order_handler;    
// }

// GetOrderHandler& OrderHandler::get_get_order_handler(){
//         return _get_order_handler;
// }

// UpdateOrderHandler& OrderHandler::get_update_order_handler(){
//         return _update_order_handler;
// }
