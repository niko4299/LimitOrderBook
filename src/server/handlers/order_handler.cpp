#include "order_handler.hpp"

OrderHandler::OrderHandler(std::shared_ptr<Exchange>& exchange): _exchange{exchange} {}


seastar::future<std::unique_ptr<seastar::http::reply>> OrderHandler::handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) {
        rep->write_body("json", seastar::json::stream_object("TEST"));
        
        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
}
