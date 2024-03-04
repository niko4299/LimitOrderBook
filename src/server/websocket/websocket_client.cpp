
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include "websocket_client.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

WebSocketClient::WebSocketClient(net::io_context& ioc, ssl::context& ctx, std::string host, std::string port,
                                 std::string subscribe_message)
    : resolver_(net::make_strand(ioc))
    , ws_{net::make_strand(ioc), ctx}
    , host_(host)
    , port_(port)
    , subscribe_message_(subscribe_message) {}

void WebSocketClient::run() {
    resolver_.async_resolve(host_, port_,
                            beast::bind_front_handler(&WebSocketClient::on_resolve, WebSocketClient::shared_from_this()));
}

void WebSocketClient::on_fail(beast::error_code ec, char const* failed_operation) {
    std::cerr << failed_operation << ": " << ec.message() << "\n";
}

void WebSocketClient::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {

    if (ec)
        return on_fail(ec, "resolve");

    // Set the timeout for the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    beast::get_lowest_layer(ws_).async_connect(
        results, beast::bind_front_handler(&WebSocketClient::on_connect, WebSocketClient::shared_from_this()));
}

void WebSocketClient::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep) {
    if (ec)
        return on_fail(ec, "connect");

    // Set a timeout on the operation
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // this line is needed for coinbase, some exchanges dont require this
    if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str())) {
        std::cerr << "SSL_set_tlsext_host_name: error " << std::endl;
        return;
    }
    // Perform the SSL handshake
    ws_.next_layer().async_handshake(ssl::stream_base::client,
                                     beast::bind_front_handler(&WebSocketClient::on_ssl_handshake, shared_from_this()));
}

void WebSocketClient::on_ssl_handshake(beast::error_code ec) {

    if (ec)
        return on_fail(ec, "ssl_handshake");

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws_).expires_never();

    // Set suggested timeout settings for the websocket
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
        req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl");
    }));

    // Perform the websocket handshake
    ws_.async_handshake(host_, "/", beast::bind_front_handler(&WebSocketClient::on_handshake, shared_from_this()));
}

void WebSocketClient::on_handshake(beast::error_code ec) {
    if (ec)
        return on_fail(ec, "handshake");

    ws_.async_write(net::buffer(subscribe_message_),
                    beast::bind_front_handler(&WebSocketClient::on_write, WebSocketClient::shared_from_this()));
}

void WebSocketClient::on_write(beast::error_code ec, std::size_t bytes_transferred) {

    boost::ignore_unused(bytes_transferred);

    if (ec){
        return on_fail(ec, "write");
    }

    ws_.async_read(buffer_, beast::bind_front_handler(&WebSocketClient::on_read, WebSocketClient::shared_from_this()));
}

void WebSocketClient::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec){
        return on_fail(ec, "read");
    }
    
    auto req = beast::buffers_to_string(buffer_.data());
    // auto request = nlohmann::json::parse(req);
    // if(request.size() == 8){
    //     std::cout<<req<<"\n";
    // }

    // if(request.size() == 9){
    //     std::cout<<req<<"\n";
    // }
    // if(request.contains("order_type")){
    //     if(request["order_type"] == "market") std::cout<<req<<"\n";
    // }
    buffer_.consume(bytes_transferred);
    ws_.async_read(buffer_, beast::bind_front_handler(&WebSocketClient::on_read, shared_from_this()));
}

void WebSocketClient::on_close(beast::error_code ec) {
    if (ec)
        return on_fail(ec, "close");

    std::cout << beast::make_printable(buffer_.data()) << std::endl;
}
