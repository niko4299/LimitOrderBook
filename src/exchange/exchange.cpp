#include "exchange.hpp"

Exchange::Exchange(std::vector<std::pair<std::string,float>>& instruments_info, std::size_t ringbuffer_size_per_instrument, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository) {
    for(std::size_t idx = 0; idx < instruments_info.size(); idx++){
        _instrument_idx[instruments_info[idx].first] = idx;
        _instruments.emplace(instruments_info[idx].first, std::make_shared<OrderBook>(instruments_info[idx].first, instruments_info[idx].second, order_repository, trade_repository));
    }
    _thread_pool = std::make_unique<ThreadPool>(instruments_info.size(), ringbuffer_size_per_instrument);
    _order_repo = order_repository;
    _trade_repo = trade_repository;
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

std::optional<std::shared_ptr<Order>> Exchange::get_order(std::string&& order_id) {
    return std::move(_order_repo->get(order_id));
}

