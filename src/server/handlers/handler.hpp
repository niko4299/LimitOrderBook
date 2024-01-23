#include <seastar/http/httpd.hh>
#include <functional>

class Handler : public seastar::httpd::handler_base {
  public:
    explicit Handler(std::function<seastar::future<std::unique_ptr<seastar::http::reply>>(std::unique_ptr<seastar::http::request>,std::unique_ptr<seastar::http::request>, std::unique_ptr<seastar::http::reply>)> handler_function): _handler_function{handler_function} {};
  private:
    seastar::future<std::unique_ptr<seastar::http::reply>> handle(const seastar::sstring& path, std::unique_ptr<seastar::http::request> req, std::unique_ptr<seastar::http::reply> rep) override {
       return _handler_function(req,rep);
    }

    std::function<seastar::future<std::unique_ptr<seastar::http::reply>>(std::unique_ptr<seastar::http::request>,std::unique_ptr<seastar::http::request>, std::unique_ptr<seastar::http::reply>)> _handler_function;
};