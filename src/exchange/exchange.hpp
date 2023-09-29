#include<unordered_map>
#include<string>
#include<memory>
#include<optional>
#include<future>

#include "../orderbook/orderbook.hpp"
#include "../utils/thread_pool.hpp"

class Exchange final {

    public:
        Exchange(std::vector<std::pair<std::string,float>>& instruments_info, std::size_t ringbuffer_size_per_instrument, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository);

        ~Exchange() = default;

        void add_order(std::shared_ptr<Order>&& order);

        void modify_order(std::shared_ptr<Order>&& order);

        void cancel_order(std::string& instrument, std::string& order_id);

        void add_instrument(std::string& instrument, std::size_t ringbuffer_size_per_instrument, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository);

        std::optional<std::shared_ptr<Order>> get_order(std::string&& order_id);

    private:
        std::unordered_map<std::string, std::uint32_t> _instrument_idx{};
        std::unordered_map<std::string, std::shared_ptr<OrderBook>> _instruments;
        std::unique_ptr<ThreadPool> _thread_pool;
        std::shared_ptr<OrderRepository> _order_repo;
        std::shared_ptr<TradeRepository> _trade_repo;
};
