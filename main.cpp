#include<iostream>
#include <functional>
#include<memory>
#include "data_structures/order.hpp"
#include "data_structures/trade.hpp"
#include "orderbook/orderbook.hpp"
#include "utils/ringbuffer.hpp"

int main(int argc, char* argv[]) {
    auto ring_buf = RingBuffer<std::function<void()>>{10};        
    std::shared_ptr<RingBuffer<std::shared_ptr<Order>>> _order_repository_ring_buffer = std::make_shared<RingBuffer<std::shared_ptr<Order>>>(1024);
    std::shared_ptr<RingBuffer<Trade>> _trade_ring_buffer = std::make_shared<RingBuffer<Trade>>(1024);

    auto orderbook = std::make_unique<OrderBook>("eth",1000.0,_trade_ring_buffer,_order_repository_ring_buffer);
    auto order = std::make_shared<Order>("1","f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::GTC, OrderType::LIMIT);
    ring_buf.push([&orderbook, order]() mutable {
        orderbook->add_order(std::move(order));
    });
    std::function<void()> f;
    ring_buf.pop(f);
    f();
    std::cout<<orderbook->get_bids().size();
}