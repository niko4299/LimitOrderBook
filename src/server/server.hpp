// #include <seastar/core/seastar.hh>
// #include <seastar/core/future-util.hh>
// #include <seastar/core/app-template.hh>
// #include <seastar/http/routes.hh>
// #include <seastar/http/request.hh>
// #include <seastar/http/function_handlers.hh>
// #include <seastar/http/httpd.hh>
// #include <seastar/core/sleep.hh>
// #include <seastar/coroutine/all.hh>
// #include <seastar/util/log.hh>
// #include <seastar/apps/lib/stop_signal.hh>
// #include <iostream>
// #include <chrono>

// using namespace std::chrono_literals;

// using namespace seastar;
// using namespace seastar::httpd;

// final class SeastarServer{

//     public:
//         SeastarServer()

// }
// logger applog{"http-requests"};

// void set_routes(routes& r) {
//     function_handler* h1 = new function_handler([](std::unique_ptr<http::request> req) -> future<json::json_return_type> {
//         applog.info("{} slow", req->get_url());
//         co_await seastar::sleep(10s);
//         co_return json::json_return_type("json-future");
//     });
//     function_handler* h2 = new function_handler([](std::unique_ptr<http::request> req) -> future<json::json_return_type> {
//         applog.info("{} fast", req->get_query_param("value"));
        
//         co_return json::json_return_type("json-future");
//     });
//     r.add(operation_type::GET, url("/slow"), h1);
//     r.add(operation_type::GET, url("/fast"), h2);
// }

// int main(int argc, char** argv) {
//     app_template app;
//     return app.run(argc, argv, [] () -> future<int> {
//         auto server = new http_server("seastar");
//         set_routes(server->_routes);
//         co_await server->listen(seastar::make_ipv4_address({1234}));

//         co_await seastar::sleep(10000s);
//         co_await server->stop();
//         delete server;
//         co_return 0;
//     });
// }
