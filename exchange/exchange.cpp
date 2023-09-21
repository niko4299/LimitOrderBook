#include "exchange.hpp"

Exchange::Exchange(const std::vector<std::string>& instruments, std::size_t ringbuffer_size_per_instrument, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository) {
    for(std::size_t idx = 0; idx < instruments.size(); idx++){
        _instrument_idx[instruments[idx]] = idx;
        _instruments.emplace(instruments[idx], std::make_shared<OrderBook>(instruments[idx], 1000, order_repository, trade_repository));
    }
    _thread_pool = std::make_unique<ThreadPool>(instruments.size(), ringbuffer_size_per_instrument);
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

void Exchange::add_instrument(std::string& instrument,std::size_t ringbuffer_size_per_instrument, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository){
        _instrument_idx[instrument] = _instruments.size();
        _instruments.emplace(instrument, std::make_shared<OrderBook>(instrument, 1000, order_repository, trade_repository));
};