#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;      
namespace http = beast::http;           
namespace websocket = beast::websocket; 
namespace net = boost::asio;  
namespace ssl = boost::asio::ssl;           
using tcp = boost::asio::ip::tcp;      

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient>{

    public:
        explicit WebSocketClient(net::io_context& ioc,ssl::context& ctx,std::string host, std::string port, std::string subscribe_message);

        virtual void run();

        virtual void on_resolve(
            beast::error_code ec,
            tcp::resolver::results_type results);

        virtual void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);

        virtual void on_ssl_handshake(beast::error_code ec);

        virtual void on_handshake(beast::error_code ec);
        
        virtual void on_write(beast::error_code ec,
            std::size_t bytes_transferred);

        virtual void on_read(beast::error_code ec,std::size_t bytes_transferred);

        virtual void on_close(beast::error_code ec);

        virtual void on_fail(beast::error_code ec, const char* error_message);

    private:
        tcp::resolver resolver_;
        websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
        beast::flat_buffer buffer_;
        std::string host_;
        std::string port_;
        std::string subscribe_message_;
};
