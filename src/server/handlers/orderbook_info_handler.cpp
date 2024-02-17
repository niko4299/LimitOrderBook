#include "orderbook_info_handler.hpp"

OrderBookInfoHandler::SnapshotHandler::SnapshotHandler(OrderBookInfoHandler& parent): _parent{parent} {}

seastar::future<std::unique_ptr<seastar::http::reply>> OrderBookInfoHandler::SnapshotHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        if (!_parent.validate_instrument_parameter(INSTRUMENT_KEY, req, rep, "invalid instrument parameter")){
                return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
        }

        auto snapshot = _parent._exchange->get_orderbook_snapshot(req->param[INSTRUMENT_KEY]);
        rep->write_body("json", seastar::json::stream_object(SeastarSnapshotJson(snapshot)));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}

bool OrderBookInfoHandler::validate_parameter(const seastar::sstring &parameter, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep, std::string message){
        bool valid_type = req->param.exists(parameter);
        if (!valid_type) {
            rep->write_body("json", seastar::json::stream_object(std::move(message)));
            rep->set_status(seastar::http::reply::status_type::bad_request);
        }

        return valid_type;
}

bool OrderBookInfoHandler::validate_instrument_parameter(const seastar::sstring &parameter, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep, std::string message){
        if (!validate_parameter(INSTRUMENT_KEY, req, rep, "invalid instrument parameter")){
                return false;
        }

        std::string instrument = req->param[INSTRUMENT_KEY];
        if (!_exchange->instrument_exists(instrument)){
                rep->set_status(seastar::http::reply::status_type::not_found);
                rep->write_body("json", seastar::json::stream_object(fmt::format("instrument {} doesn't exist", instrument)));

                return false;
        }

        return true;
}

OrderBookInfoHandler::OrderBookInfoHandler(std::shared_ptr<Exchange>& exchange): _snapshot_handler(*this), _exchange{exchange} {}
