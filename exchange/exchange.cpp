#include "exchange.hpp"

Exchange::Exchange(const std::vector<std::string>& symbols, std::size_t ringbuffer_size_per_instrument, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository) {
    for(auto idx = 0; idx < symbols.size(); idx++){
        _instrument_idx[symbols[idx]] = idx;
        _instruments.emplace(symbols[idx], std::make_shared<OrderBook>(symbols[idx], 1000, order_repository, trade_repository));
    }
    _thread_pool = std::make_unique<ThreadPool>(symbols.size(), ringbuffer_size_per_instrument);
};

void Exchange::add_order(std::shared_ptr<Order>&& order){
    auto instrument = order->get_instrument();
    auto& orderbook = _instruments[instrument];
    auto thread_id = _instrument_idx[instrument];

    _thread_pool->enqueue(thread_id, [&orderbook, &order]() {
        orderbook->add_order(std::move(order));
    });
};

void Exchange::modify_order(std::shared_ptr<Order>&& order){
    auto instrument = order->get_instrument();
    auto& orderbook = _instruments[instrument];
    auto thread_id = _instrument_idx[instrument];

    _thread_pool->enqueue(thread_id, [&orderbook, &order]() {
        orderbook->modify_order(std::move(order));
    });
};

void Exchange::cancel_order(std::string& instrument, std::string& order_id){
    auto& orderbook = _instruments[instrument];
    auto thread_id = _instrument_idx[instrument];

    _thread_pool->enqueue(thread_id, [&orderbook, &order_id]() {
        orderbook->cancel_order(std::move(order_id));
    });
};

void Exchange::add_instrument(std::string& instrument,std::size_t ringbuffer_size_per_instrument){

};