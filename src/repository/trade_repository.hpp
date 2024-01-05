#pragma once

#include <optional>
#include <thread>
#include <stdexcept>

#include <cassandra.h>

#include "../data_structures/trade.hpp"
#include "../utils/ringbuffer.hpp"

class TradeRepository{
    
    public:
        TradeRepository(std::string&& hosts, uint32_t batch_size, std::size_t ringbuffer_size);
        
        TradeRepository() = default;

        ~TradeRepository();

        void process_messages(std::stop_token s);

        bool save(Trade trade);

        std::vector<Trade> get_all_trades();

        std::optional<Trade> get_trade_by_primary_key(std::string&& buyer_id, std::string&& seller_id, std::time_t timestamp);

        bool run_query(std::string&& query);

        void enqueue(Trade& trade) const;
        
        TradeRepository(TradeRepository const &) = delete;
        TradeRepository(TradeRepository &&) = delete;
        TradeRepository &operator=(TradeRepository const &) = delete;
        TradeRepository &operator=(TradeRepository &&) = delete;

    private:
    
        CassCluster* _cluster;
        CassSession* _session;
        uint32_t _batch_size;
        bool _done{false};
        std::unique_ptr<RingBuffer<Trade>> _ring_buffer;
        const CassPrepared* _prepared_insert_query;
        const char* _query_all = "SELECT * FROM orderbook.trades";
        const CassPrepared* _prepared_select_by_primary_key_query;
        std::jthread _thread; 
};
