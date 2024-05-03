#include "exchange.hpp"

Exchange::Exchange(
    std::vector<std::pair<std::string,float>>& instruments_info,
    std::size_t ringbuffer_size_per_instrument,
    std::shared_ptr<OrderRepository>& order_repository,
    std::shared_ptr<TradeRepository>& trade_repository,
    bool ignore_working_hours
) {
    for(std::size_t idx = 0; idx < instruments_info.size(); idx++){
        _instrument_idx[instruments_info[idx].first] = idx;
        _instruments.emplace(instruments_info[idx].first, std::make_shared<OrderBook>(instruments_info[idx].first, instruments_info[idx].second, order_repository, trade_repository));
    }
    _thread_pool = std::make_unique<ThreadPool>(instruments_info.size(), ringbuffer_size_per_instrument);
    _order_repo = order_repository;
    _trade_repo = trade_repository;
    if(!ignore_working_hours){
        _is_working_thread =  std::jthread(std::bind_front(&Exchange::is_working_thread, this));
    }else{
        _working = true;
    }
};

Exchange::~Exchange(){
    if(_is_working_thread.joinable()){
        _is_working_thread.request_stop();
        _is_working_thread.join();
    }
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

// TODO: Refactor this to use snapshot repository (when implemented) and not orderbook directly.
Snapshot Exchange::get_orderbook_snapshot(std::string_view instrument){
    auto& orderbook = _instruments[instrument];

    return orderbook->get_snapshot();
}

std::future<OrderStatus> Exchange::enqueue_task(std::uint32_t& thread_id, std::function<OrderStatus(void)>&& task_function){
    auto task = std::packaged_task<OrderStatus()>(
        std::move(task_function)
    );

    auto future = task.get_future();

    _thread_pool->enqueue(thread_id,[t = std::make_shared<std::packaged_task<OrderStatus(void)>>(std::move(task))]() {(*t)();});

    return future;
}

bool Exchange::is_working(){
    return _working.load();
}

void Exchange::is_working_thread(std::stop_token s){
    while(!s.stop_requested()){
        std::time_t now = time(0);
        std::tm* local_time = localtime(&now);

        int curr_h = local_time->tm_hour;
        int curr_m = local_time->tm_min;

        std::chrono::system_clock::time_point now_time_point = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point next_time_point;

        if (curr_h < opening_h || (curr_h == opening_h && curr_m < opening_m)) {
            std::tm target_time = *local_time;
            target_time.tm_hour = opening_h;
            target_time.tm_min = opening_m;
            target_time.tm_sec = 0;
            next_time_point = std::chrono::system_clock::from_time_t(std::mktime(&target_time));
        } else if (curr_h < closing_h || (curr_h == closing_h && curr_m < closing_m)) {
            std::tm target_time = *local_time;
            target_time.tm_hour = opening_h;
            target_time.tm_min = opening_m;
            target_time.tm_sec = 0;
            next_time_point = std::chrono::system_clock::from_time_t(std::mktime(&target_time));
        } else {
            std::tm target_time = *local_time;
            auto dow = day_of_week(target_time.tm_mday, target_time.tm_mon, target_time.tm_year);
            if(dow == 5 ){
                target_time.tm_mday += 3;  // friday move 3 days
            }else if(dow == 6){
                target_time.tm_mday += 2;  // saturday move 2 days
            } else{
                target_time.tm_mday += 1;
            }

            target_time.tm_hour = opening_h;
            target_time.tm_min = opening_m;
            target_time.tm_sec = 0;
            next_time_point = std::chrono::system_clock::from_time_t(mktime(&target_time));
        }

        std::chrono::duration<double> sleep_duration = next_time_point - now_time_point;

        std::this_thread::sleep_for(sleep_duration);

        if (curr_h == opening_h && curr_m == opening_m) {
            _working = true;
        } else if (curr_h == closing_h && curr_m == closing_m) {
            _working = false; 
            check_date_orders();
        }
    }
};

void Exchange::check_date_orders(){
    for(auto&[instrument, orderbook]: _instruments){
        orderbook->check_date_orders();
    }
};
