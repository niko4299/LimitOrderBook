#include <optional>
#include <filesystem>

#include "rocksdb/db.h"
#include "rocksdb/options.h"

#include "../data_structures/trade.hpp"

class TradeRepository{
    
    public:
        TradeRepository(std::string& db_path, uint32_t batch_size);

        ~TradeRepository();

        bool save(std::vector<Trade> order);

        bool save(Trade trade);

        std::vector<Trade> get_all_trades_between(std::time_t starting_timestamp, std::time_t ending_time_stamp);

    private:
        rocksdb::DB* _db;
        uint32_t _batch_size;
};