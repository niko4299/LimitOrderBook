#include <memory>

#include <seastar/http/httpd.hh>

#include "../../exchange/exchange.hpp"

class OrderHandler : public seastar::httpd::handler_base {
  public:
    explicit OrderHandler(std::shared_ptr<Exchange>& exchange);
  
  private:
    seastar::future<std::unique_ptr<seastar::http::reply>> handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) override;

    std::shared_ptr<Exchange>& _exchange;
};