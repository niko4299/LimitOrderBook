#include <memory>

#include <seastar/http/httpd.hh>
#include <seastar/http/json_path.hh>
#include <simdjson.h>

#include "../../exchange/exchange.hpp"
#include "response/snapshot_response.hpp"
#include "consts.hpp"

class OrderBookInfoHandler {
    public:
        explicit OrderBookInfoHandler(std::shared_ptr<Exchange>& exchange);
    private:
        class SnapshotHandler : public seastar::httpd::handler_base{
            public:
                explicit SnapshotHandler(OrderBookInfoHandler& parent);
            private:
                seastar::future<std::unique_ptr<seastar::http::reply>> handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) override;

                OrderBookInfoHandler& _parent;
        };
    public:
        SnapshotHandler _snapshot_handler;
        
    private:
        bool validate_parameter(const seastar::sstring &parameter, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep, std::string message);

        bool validate_instrument_parameter(const seastar::sstring &parameter, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep, std::string message);

        std::shared_ptr<Exchange>& _exchange;
};