#include <memory>

#include <seastar/http/httpd.hh>
#include <seastar/http/json_path.hh>
#include <simdjson.h>

#include "../../exchange/exchange.hpp"
#include "../../utils/uuid_generator.hpp"
#include "../../mappers/orderjson_order_mapper.hpp"
#include "../response/order_response.hpp"

static inline const seastar::sstring INSTRUMENT_KEY = seastar::sstring("instrument");
static inline const seastar::sstring ORDER_ID_KEY = seastar::sstring("order_id");

class OrderHandler {
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


  public:
    explicit OrderHandler(std::shared_ptr<Exchange>& exchange);

    // NewOrderHandler& get_new_order_handler();

    // GetOrderHandler& get_get_order_handler();

    // UpdateOrderHandler& get_update_order_handler();

    CreateOrderHandler _create_order_handler;
    UpdateOrderHandler _update_order_handler;
    GetOrderHandler _get_order_handler;
    CancelOrderHandler _cancel_order_handler;
  private:

    std::shared_ptr<Exchange>& _exchange;
    UUIDGenerator _uuid_generator;
    OrderJsonOrderMapper _order_mapper;
};
