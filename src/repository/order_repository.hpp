#pragma once

#include <optional>
#include <filesystem>
#include <thread>

#include <rocksdb/comparator.h>
#include <rocksdb/db.h>
#include <rocksdb/iterator.h>
#include <rocksdb/options.h>
#include <rocksdb/status.h>
#include <rocksdb/utilities/transaction_db.h>

#include "../data_structures/order.hpp"
#include "../utils/ringbuffer.hpp"


constexpr const char *OrderColumnFamily = "order";
constexpr const char *DefaultColumnFamily = "default";

class OrderRepository{
    
    public:
        
        OrderRepository(std::string&& db_path, std::size_t ringbuffer_size);

        OrderRepository() = default;
        ~OrderRepository();

        void process_messages(std::stop_token s);

        bool save(std::shared_ptr<Order>& order);

        std::optional<std::shared_ptr<Order>> get(std::string_view order_id);

        void enqueue(std::shared_ptr<Order>& order) const;

        OrderRepository(OrderRepository const &) = delete;
        OrderRepository(OrderRepository &&) = delete;
        OrderRepository &operator=(OrderRepository const &) = delete;
        OrderRepository &operator=(OrderRepository &&) = delete;

    private:
        rocksdb::TransactionDB* _db;
        rocksdb::Options _options;
        rocksdb::ColumnFamilyHandle* _order_handler = nullptr;
        rocksdb::ColumnFamilyHandle* _default_handler = nullptr;
        std::unique_ptr<RingBuffer<std::shared_ptr<Order>>> _ring_buffer;
        std::jthread _thread;
};
