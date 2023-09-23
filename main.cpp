#include<iostream>
#include <functional>
#include<memory>
#include "src/data_structures/order.hpp"
#include "src/data_structures/trade.hpp"
#include "src/orderbook/orderbook.hpp"
#include "src/utils/ringbuffer.hpp"

int main(int argc, char* argv[]) {
    auto ring_buf = RingBuffer<std::function<void()>>{10};        
    auto order_repo = std::make_shared<OrderRepository>("./db_path",1024);
    auto trade_repo = std::make_shared<TradeRepository>("0.0.0.0",100,1024);

    auto orderbook = std::make_unique<OrderBook>("eth",1000.0,order_repo,trade_repo);
    auto order = std::make_shared<Order>("1","f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::GTC, OrderType::LIMIT);
    ring_buf.push([&orderbook, order]() mutable {
        orderbook->add_order(std::move(order));
    });
    std::function<void()> f;
    ring_buf.pop(f);
    f();
    std::cout<<orderbook->get_bids().size();
}
