#include<boost/beast/core.hpp>
#include<boost/beast/websocket.hpp>
#include<boost/asio/dispatch.hpp>
#include<boost/asio/strand.hpp>
#include<memory>
#include<string>
#include<iostream>
// #include "../exchange/exchange.hpp"
// #include "../exchange/message_types.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace ssl = boost::asio::ssl;  

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>{

    public:
        explicit WebSocketSession(tcp::socket&& socket,int buy_treshold, int sell_treshold);

        virtual void run();

        virtual void on_run();

        virtual void on_accept(boost::beast::error_code ec);

        virtual void do_read();
        
        virtual void on_write(beast::error_code ec, std::size_t bytes_transferred);

        virtual void on_read(beast::error_code ec,std::size_t bytes_transferred);

        virtual void on_fail(beast::error_code ec, const char* error_message);

    private:

        // trader personal exchange.
        // std::unique_ptr<Exchange> exchange_;
        websocket::stream<beast::tcp_stream> ws_;        
        beast::flat_buffer buffer_;
};

class WebSocketListener : public std::enable_shared_from_this<WebSocketListener>{
public:
    WebSocketListener(net::io_context& ioc, std::string address, int port);

    virtual void on_fail(beast::error_code ec, const char* error_message);

    virtual void run();

    virtual void do_accept();

    virtual void on_accept(beast::error_code ec, tcp::socket socket);

private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
};
