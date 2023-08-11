#include <optional>
#include <filesystem>

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include <boost/lockfree/spsc_queue.hpp>

#include "../data_structures/trade.hpp"
#include "../utils/convert_time_to_string.hpp"

class TradeRepository{
    
    public:
        TradeRepository(std::string& db_path, uint32_t batch_size,  std::shared_ptr<boost::lockfree::spsc_queue<Trade, boost::lockfree::capacity<1024UL>>>& ring_buffer);

        ~TradeRepository();

        bool save(std::vector<Trade> order);

        bool save(Trade trade);

        void process_message(std::stop_token s);
        
        // std::vector<Trade> get_all_trades_between(std::time_t starting_timestamp, std::time_t ending_time_stamp);

    private:
        rocksdb::DB* _db;
        uint32_t _batch_size;
        bool _done{false};
        std::shared_ptr<boost::lockfree::spsc_queue<Trade, boost::lockfree::capacity<1024UL>>> _ring_buffer;
};