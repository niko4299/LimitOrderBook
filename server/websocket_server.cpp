#include "websocket_server.hpp"

WebSocketSession::WebSocketSession(tcp::socket&& socket,int buy_treshold,int sell_treshold): ws_(std::move(socket)) {
    // exchange_ = std::make_unique<Exchange>(buy_treshold,sell_treshold);
}

// Print out error.
void WebSocketSession::on_fail(beast::error_code ec, char const* error_message){
    std::cerr << error_message << ": " << ec.message() << "\n";
}

// Run WebSocketSession.
void WebSocketSession::run(){
    net::dispatch(ws_.get_executor(),
                beast::bind_front_handler(
                    &WebSocketSession::on_run,
                    shared_from_this()));
        }

void WebSocketSession::on_run(){
    ws_.set_option(
        websocket::stream_base::timeout::suggested(
            beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::response_type& res)
        {
            res.set(http::field::server,
                std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-server-async");
        }));
    // Accept the websocket handshake
    ws_.async_accept(
        beast::bind_front_handler(
            &WebSocketSession::on_accept,
            shared_from_this()));
}


void WebSocketSession::on_accept(beast::error_code ec){
    if(ec) return on_fail(ec, "accept");
    do_read();
}

void WebSocketSession::do_read(){
    ws_.async_read(
        buffer_,
        beast::bind_front_handler(
            &WebSocketSession::on_read,
            shared_from_this()));
}

// If closed destroy everything else check if message has too much bytes if yes return empty string else process message.
void WebSocketSession::on_read(beast::error_code ec, std::size_t bytes_transferred){
    boost::ignore_unused(bytes_transferred);

    // This indicates that the session was closed
    if(ec == websocket::error::closed) return;

    if(ec) return on_fail(ec, "read");
    
    if(bytes_transferred > 54){
        ws_.async_write(
        net::buffer("Wrong message!"),
        beast::bind_front_handler(
            &WebSocketSession::on_write,
            shared_from_this()));
    }else{
        
    // auto response = exchange_->process_message(beast::buffers_to_string(buffer_.data()));
    std::string response = "2";
    ws_.async_write(
        net::buffer(response),
        beast::bind_front_handler(
            &WebSocketSession::on_write,
            shared_from_this()));
    }

}

void WebSocketSession::on_write(beast::error_code ec,std::size_t bytes_transferred){
    boost::ignore_unused(bytes_transferred);

    if(ec)return on_fail(ec, "write");

    buffer_.consume(buffer_.size());

    do_read();
}

WebSocketListener::WebSocketListener(net::io_context& ioc, std::string address, int port): 
    ioc_(ioc), acceptor_(ioc){
    beast::error_code ec;
    tcp::endpoint endpoint{net::ip::make_address(address), static_cast<unsigned short>(port)};    
    acceptor_.open(endpoint.protocol(), ec);

    if(ec){
        on_fail(ec, "open");
        return;
    }

    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if(ec){
        on_fail(ec, "set_option");
        return;
    }

    acceptor_.bind(endpoint, ec);
    if(ec){
        on_fail(ec, "bind");
        return;
    }
    acceptor_.listen(net::socket_base::max_listen_connections, ec);

    if(ec){
        on_fail(ec, "listen");
        return;
    }
}

void WebSocketListener::on_fail(beast::error_code ec, char const* error_message){
    std::cerr << error_message << ": " << ec.message() << "\n";
}

void WebSocketListener::run(){
    do_accept();
}

void WebSocketListener::do_accept(){
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &WebSocketListener::on_accept,
            shared_from_this()));
}

// On new connection create WebSocketSession.
void WebSocketListener::on_accept(beast::error_code ec, tcp::socket socket){
    if(ec) on_fail(ec, "accept");
    std::make_shared<WebSocketSession>(std::move(socket),10,10)->run();
    do_accept();
}
