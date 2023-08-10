#include <optional>
#include <filesystem>

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include <boost/lockfree/spsc_queue.hpp>

#include "../data_structures/order.hpp"

class OrderRepository{
    
    public:
        
        OrderRepository(std::string& db_path,std::shared_ptr<boost::lockfree::spsc_queue<std::shared_ptr<Order>, boost::lockfree::capacity<1024>>>& ring_buffer);

        ~OrderRepository();

        void process_message(std::stop_token s);

        bool save(std::shared_ptr<Order>& order);

        std::optional<std::shared_ptr<Order>> get(std::string& order_id);

    private:
        rocksdb::DB* _db;
        std::shared_ptr<boost::lockfree::spsc_queue<std::shared_ptr<Order>, boost::lockfree::capacity<1024UL>>> _ring_buffer;
};