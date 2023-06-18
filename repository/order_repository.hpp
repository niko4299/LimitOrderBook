#include <optional>
#include <filesystem>

#include "rocksdb/db.h"
#include "rocksdb/options.h"

#include "../data_structures/order.hpp"

class OrderRepository{
    
    public:

        OrderRepository(std::string& db_path);

        ~OrderRepository();

        bool save(std::shared_ptr<Order>& order);

        std::optional<std::shared_ptr<Order>> get(std::string& order_id);

    private:
        rocksdb::DB* _db;
};