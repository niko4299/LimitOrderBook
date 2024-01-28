#include <memory>

#include <seastar/http/httpd.hh>

#include "../../exchange/exchange.hpp"

class OrderHandler {
    class NewOrderHandler : public seastar::httpd::handler_base{
      public:
          explicit NewOrderHandler(OrderHandler& parent);
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


  public:
    explicit OrderHandler(std::shared_ptr<Exchange>& exchange);

    // NewOrderHandler& get_new_order_handler();

    // GetOrderHandler& get_get_order_handler();

    // UpdateOrderHandler& get_update_order_handler();

    NewOrderHandler _new_order_handler;
    UpdateOrderHandler _update_order_handler;
    GetOrderHandler _get_order_handler;
  private:

    std::shared_ptr<Exchange>& _exchange;
};
