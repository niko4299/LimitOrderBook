#include "order_handler.hpp"

OrderHandler::CreateOrderHandler::CreateOrderHandler(OrderHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::CreateOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        if (!_parent.validate_instrument_parameter(INSTRUMENT_KEY, req, rep)){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        simdjson::padded_string json(req->content.c_str(),req->content.size()); 
        simdjson::ondemand::document doc = _parent._parser.iterate(json);
        auto order = _parent._order_mapper.map_json_to_order(doc);

        if(!_parent.validate_order_side(order->get_side(),req,rep)){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        if (!_parent.check_if_exchange_is_working(rep)){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        order->set_id(_parent._uuid_generator.generate());
        auto order_status = _parent._exchange->add_order(std::move(req->get_path_param(INSTRUMENT_KEY)), std::move(order));
        
        rep->write_body("json", seastar::json::stream_object(SeastarOrderInfoJson(order->get_id(), order_status)));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}

OrderHandler::GetOrderHandler::GetOrderHandler(OrderHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::GetOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        if (!_parent.validate_parameter(ORDER_ID_KEY, req, rep, "invalid order parameter")){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        if (!_parent.check_if_exchange_is_working(rep)){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        auto order = _parent._exchange->get_order(std::move(req->get_path_param(ORDER_ID_KEY)));
        if(!order.has_value()){
                rep->set_status(seastar::http::reply::status_type::not_found);
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        rep->set_status(seastar::http::reply::status_type::ok);
        rep->write_body("json", seastar::json::stream_object(SeastarOrderJson(*order)));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));

}

OrderHandler::CancelOrderHandler::CancelOrderHandler(OrderHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::CancelOrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        if (!_parent.validate_parameter(ORDER_ID_KEY, req, rep, "invalid order parameter")){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }
        
        if (!_parent.check_if_exchange_is_working(rep)){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }
        
        if (!_parent.validate_instrument_parameter(INSTRUMENT_KEY, req, rep)){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }


        auto order_status = _parent._exchange->cancel_order(std::move(req->get_path_param(INSTRUMENT_KEY)), std::move(req->get_path_param(ORDER_ID_KEY)));
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
        if (!_parent.validate_instrument_parameter(INSTRUMENT_KEY, req, rep)){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        if (!_parent.check_if_exchange_is_working(rep)){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        simdjson::padded_string json(req->content.c_str(),req->content.size()); 
        simdjson::ondemand::document doc = _parent._parser.iterate(json);
        auto order = _parent._order_mapper.map_json_to_order(doc);
        
        auto order_status = _parent._exchange->modify_order(std::move(req->get_path_param(INSTRUMENT_KEY)), std::move(order));
        rep->write_body("json", seastar::json::stream_object(SeastarOrderInfoJson(order->get_id(), order_status)));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}

OrderHandler::OrderHandler(std::shared_ptr<Exchange>& exchange): _create_order_handler(*this), _update_order_handler(*this), _get_order_handler(*this), _cancel_order_handler(*this), _exchange{exchange}, _uuid_generator{}, _order_mapper{} {} 

bool OrderHandler::validate_parameter(const seastar::sstring &parameter, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep, std::string message){
        bool valid_type = req->param.exists(parameter);
        if (!valid_type) {
            rep->write_body("json", seastar::json::stream_object(std::move(message)));
            rep->set_status(seastar::http::reply::status_type::bad_request);
        }

        return valid_type;
}

bool OrderHandler::validate_instrument_parameter(const seastar::sstring &parameter, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep){
        if (!validate_parameter(INSTRUMENT_KEY, req, rep, "invalid instrument parameter")){
                return false;
        }

        std::string instrument = req->get_path_param(INSTRUMENT_KEY);
        if (!_exchange->instrument_exists(instrument)){
                rep->set_status(seastar::http::reply::status_type::not_found);
                rep->write_body("json", seastar::json::stream_object(fmt::format("instrument {} doesn't exist", instrument)));

                return false;
        }

        return true;
}

bool OrderHandler::validate_order_side(Side order_side, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep){
        if (order_side == Side::UNKOWN){
                rep->set_status(seastar::http::reply::status_type::bad_request);
                rep->write_body("json", seastar::json::stream_object("order side can only be BUY or SELL"));

                return false;
        }

        return true;
}

bool OrderHandler::check_if_exchange_is_working(std::unique_ptr<seastar::http::reply> &rep){
        if (!_exchange->is_working()){
                rep->set_status(seastar::http::reply::status_type::no_content);
                rep->write_body("json", seastar::json::stream_object("exchange is not working currently."));

                return false;
        }

        return true;
}
