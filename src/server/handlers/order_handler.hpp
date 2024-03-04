#include <memory>

#include <seastar/http/httpd.hh>
#include <seastar/http/json_path.hh>
#include <simdjson.h>

#include "../../exchange/exchange.hpp"
#include "../../utils/uuid_generator.hpp"
#include "../../mappers/orderjson_order_mapper.hpp"
#include "./response/order_response.hpp"
#include "consts.hpp"

class OrderHandler {
  public: 
    explicit OrderHandler(std::shared_ptr<Exchange>& exchange);

  private:
    class CreateOrderHandler : public seastar::httpd::handler_base{
      public:
          explicit CreateOrderHandler(OrderHandler& parent);
      private:
        seastar::future<std::unique_ptr<seastar::http::reply>> handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) override;

        OrderHandler& _parent;
    };

    class UpdateOrderHandler : public seastar::httpd::handler_base{
      public:
          explicit UpdateOrderHandler(OrderHandler& parent);
      private:
        seastar::future<std::unique_ptr<seastar::http::reply>> handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) override;

        OrderHandler& _parent;
    };

    class GetOrderHandler : public seastar::httpd::handler_base{
      public:
          explicit GetOrderHandler(OrderHandler& parent);
      private:
        seastar::future<std::unique_ptr<seastar::http::reply>> handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) override;

        OrderHandler& _parent;
    };

    class CancelOrderHandler : public seastar::httpd::handler_base{
      public:
          explicit CancelOrderHandler(OrderHandler& parent);
      private:
        seastar::future<std::unique_ptr<seastar::http::reply>> handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) override;

        OrderHandler& _parent;
    };

    bool validate_parameter(const seastar::sstring &parameter, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep, std::string message);

    bool validate_instrument_parameter(const seastar::sstring &parameter, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep, std::string message);

    bool validate_order_side(Side order_side, std::unique_ptr<seastar::http::request> &req, std::unique_ptr<seastar::http::reply> &rep, std::string message);

  public:

    CreateOrderHandler _create_order_handler;
    UpdateOrderHandler _update_order_handler;
    GetOrderHandler _get_order_handler;
    CancelOrderHandler _cancel_order_handler;
  private:

    std::shared_ptr<Exchange>& _exchange;
    UUIDGenerator _uuid_generator;
    OrderJsonOrderMapper _order_mapper;
    simdjson::ondemand::parser _parser;
};
