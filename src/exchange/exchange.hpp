#pragma once

#include<unordered_map>
#include<string>
#include<memory>
#include<optional>
#include<future>
#include<chrono>
#include<ctime>
#include<chrono>

#include "../orderbook/orderbook.hpp"
#include "../utils/thread_pool.hpp"
#include "../data_structures/enums.hpp"
#include "../utils/time.hpp"

class Exchange final {
    static constexpr int opening_h = 9;
    static constexpr int opening_m = 30;

    static constexpr int closing_h = 17;
    static constexpr int closing_m = 30;

    public:
        Exchange(
            std::vector<std::pair<std::string,float>>& instruments_info, 
            std::size_t ringbuffer_size_per_instrument,
            std::shared_ptr<OrderRepository>& order_repository, 
            std::shared_ptr<TradeRepository>& trade_repository,
            bool ignore_working_hours = false
        );

        ~Exchange();

        OrderStatus add_order(std::string_view instrument, std::shared_ptr<Order>&& order);

        OrderStatus modify_order(std::string_view instrument, std::shared_ptr<Order>&& order);

        OrderStatus cancel_order(std::string_view instrument, std::string_view order_id);

        bool add_instrument(std::string_view instrument, float starting_price);

        bool instrument_exists(std::string_view instrument);

        bool is_working();

        std::optional<std::shared_ptr<Order>> get_order(std::string_view order_id);

        Snapshot get_orderbook_snapshot(std::string_view instrument);


    private:
        std::future<OrderStatus> enqueue_task(std::uint32_t& thread_id, std::function<OrderStatus(void)>&& task_function);
       
        void check_date_orders();

        void is_working_thread(std::stop_token s);

        std::unordered_map<std::string_view, std::uint32_t> _instrument_idx{};
        std::unordered_map<std::string_view, std::shared_ptr<OrderBook>> _instruments;
        std::unique_ptr<ThreadPool> _thread_pool;
        std::shared_ptr<OrderRepository> _order_repo;
        std::shared_ptr<TradeRepository> _trade_repo;
        std::jthread _is_working_thread;
        std::atomic<bool> _working{false};
};
