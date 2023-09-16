#include <optional>
#include <filesystem>

#include "rocksdb/db.h"
#include "rocksdb/options.h"

#include "../data_structures/order.hpp"
#include "../utils/ringbuffer.hpp"

class OrderRepository{
    
    public:
        
        OrderRepository(std::string& db_path,std::shared_ptr<RingBuffer<std::shared_ptr<Order>>>& ring_buffer);

        ~OrderRepository();

        void process_message(std::stop_token s);

        bool save(std::shared_ptr<Order>& order);

        std::optional<std::shared_ptr<Order>> get(std::string& order_id);

    private:
        rocksdb::DB* _db;
        std::shared_ptr<RingBuffer<std::shared_ptr<Order>>> _ring_buffer;
};