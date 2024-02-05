#include "order_handler.hpp"

OrderHandler::OrderHandler(std::shared_ptr<Exchange>& exchange): _new_order_handler(*this), _update_order_handler(*this), _get_order_handler(*this), _cancel_order_handler(*this), _exchange{exchange}, _uuid_generator{}, _order_mapper{} {} 


OrderHandler::NewOrderHandler::NewOrderHandler(OrderHandler& parent): _parent{parent} {}


seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::NewOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        simdjson::ondemand::parser parser;
        std::string instrument = req->param[INSTRUMENT_KEY];
        simdjson::padded_string json(req->content.c_str(),req->content.size()); 
        simdjson::ondemand::document doc = parser.iterate(json);
        auto order = _parent._order_mapper.map_json_to_order(doc);
        order->set_id(_parent._uuid_generator.generate());
        auto order_status = _parent._exchange->add_order(instrument, std::move(order));
        
        rep->write_body("json", seastar::json::stream_object(SeastarOrderInfoJson(order->get_id(), order_status)));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}

OrderHandler::GetOrderHandler::GetOrderHandler(OrderHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::GetOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        std::string order_id = req->param[ORDER_ID_KEY];
        auto order = _parent._exchange->get_order(std::move(order_id));
        if(!order.has_value()){
                rep->set_status(seastar::http::reply::status_type::not_found);
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        std::ostringstream oss;

        rep->set_status(seastar::http::reply::status_type::ok);
        rep->write_body("json", seastar::json::stream_object(SeastarOrderJson(*order)));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));

}

OrderHandler::CancelOrderHandler::CancelOrderHandler(OrderHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::CancelOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        std::string order_id = req->param[ORDER_ID_KEY];
        std::string instrument = req->param[INSTRUMENT_KEY];
        auto order_status = _parent._exchange->cancel_order(instrument, order_id);

        if(order_status == OrderStatus::NOT_FOUND){
                rep->set_status(seastar::http::reply::status_type::not_found);
        }else if(order_status == OrderStatus::CANCELLED) {
                rep->set_status(seastar::http::reply::status_type::ok);
        }else if(order_status == OrderStatus::REJECTED){
                rep->set_status(seastar::http::reply::status_type::not_acceptable);
        }

        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}

OrderHandler::UpdateOrderHandler::UpdateOrderHandler(OrderHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::UpdateOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        simdjson::ondemand::parser parser;
        std::string instrument = req->param[INSTRUMENT_KEY];
        simdjson::padded_string json(req->content.c_str(),req->content.size()); 
        simdjson::ondemand::document doc = parser.iterate(json);
        auto order = _parent._order_mapper.map_json_to_order(doc);
        order->set_id(_parent._uuid_generator.generate());
        auto order_status = _parent._exchange->modify_order(instrument, std::move(order));
        
        rep->write_body("json", seastar::json::stream_object(SeastarOrderInfoJson(order->get_id(), order_status)));
        
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
