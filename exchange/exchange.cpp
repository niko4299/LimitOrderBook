#include "exchange.hpp"

Exchange::Exchange(const std::vector<std::string>& symbols, std::size_t ringbuffer_size_per_instrument, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository) {
    for(auto i = 0; i < symbols.size(); i++){
        auto orderbook = std::make_shared<OrderBook>(symbols[i], 1000, order_repository, trade_repository);
    }
};