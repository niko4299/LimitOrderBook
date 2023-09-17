#include<unordered_map>
#include<string>
#include<memory>

#include "../orderbook/orderbook.hpp"
#include "../utils/thread_pool.hpp"

class Exchange final {

    public:
        explicit Exchange(const std::vector<std::string>& symbols, std::size_t ringbuffer_size_per_instrument, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository);

        ~Exchange() = default;

        void add_order();

        void modify_order();

        void cancel_order();

        void add_instrument();

    private:
        std::unordered_map<std::string, std::shared_ptr<OrderBook>> _instruments;
        std::unique_ptr<ThreadPool> thread_pool;
};