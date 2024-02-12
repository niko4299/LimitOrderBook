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

OrderStatus Exchange::add_order(std::string_view instrument, std::shared_ptr<Order>&& order){
    auto& orderbook = _instruments[instrument];
    auto thread_id = _instrument_idx[instrument];

    auto future = enqueue_task(thread_id, [&orderbook, &order](){
            return orderbook->add_order(std::move(order));;
        });

    return future.get();
};

OrderStatus Exchange::modify_order(std::string_view instrument, std::shared_ptr<Order>&& order){
    auto& orderbook = _instruments[instrument];
    auto thread_id = _instrument_idx[instrument];

    auto future = enqueue_task(thread_id, [&orderbook, &order]() {
                return orderbook->modify_order(std::move(order));;
            });

    return future.get();
};

OrderStatus Exchange::cancel_order(std::string_view instrument, std::string_view order_id){
    auto& orderbook = _instruments[instrument];
    auto thread_id = _instrument_idx[instrument];

    auto future = enqueue_task(thread_id, [&orderbook, &order_id]() {
        return orderbook->cancel_order(std::move(order_id));
    });

    return future.get();
};

bool Exchange::add_instrument(std::string_view instrument, float starting_price){
    if (instrument_exists(instrument)){
        return false;
    }

    _instrument_idx[instrument] = _instruments.size();
    _instruments.emplace(instrument, std::make_shared<OrderBook>(instrument, starting_price, _order_repo, _trade_repo));
    
    return true;
};

bool Exchange::instrument_exists(std::string_view instrument){
    if(_instruments.find(instrument) != _instruments.end()){
        return true;
    }

    return false;
}

std::optional<std::shared_ptr<Order>> Exchange::get_order(std::string_view order_id) {
    return std::move(_order_repo->get(order_id));
}

std::future<OrderStatus> Exchange::enqueue_task(std::uint32_t& thread_id, std::function<OrderStatus(void)>&& task_function){
    auto task = std::packaged_task<OrderStatus()>(
        std::move(task_function)
    );

    auto future = task.get_future();

    _thread_pool->enqueue(thread_id,[t = std::make_shared<std::packaged_task<OrderStatus(void)>>(std::move(task))]() {(*t)();});

    return future;
}

