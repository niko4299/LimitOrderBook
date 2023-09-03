#include <optional>
#include <filesystem>
#include <thread>
#include <stdexcept>

#include <cassandra.h>
#include <boost/lockfree/spsc_queue.hpp>

#include "../data_structures/trade.hpp"
#include "../utils/convert_time_to_string.hpp"

class TradeRepository{
    
    public:
        TradeRepository(std::string& hosts, uint32_t batch_size,  std::shared_ptr<boost::lockfree::spsc_queue<Trade, boost::lockfree::capacity<1024UL>>>& ring_buffer);

        ~TradeRepository();

        bool save(Trade trade);

        void process_message(std::stop_token s);
        
    private:
        CassCluster* _cluster;
        CassSession* _session;
        uint32_t _batch_size;
        bool _done{false};
        std::shared_ptr<boost::lockfree::spsc_queue<Trade, boost::lockfree::capacity<1024UL>>> _ring_buffer;
        const CassPrepared* _prepared_insert_query;
};