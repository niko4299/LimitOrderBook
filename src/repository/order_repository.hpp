#pragma once

#include <optional>
#include <filesystem>
#include <thread>

#include "rocksdb/db.h"
#include "rocksdb/options.h"

#include "../data_structures/order.hpp"
#include "../utils/ringbuffer.hpp"

class OrderRepository{
    
    public:
        
        OrderRepository(std::string&& db_path, std::size_t ringbuffer_size);

        OrderRepository() = default;
        ~OrderRepository();

        void process_messages(std::stop_token s);

        bool save(std::shared_ptr<Order>& order);

        std::optional<std::shared_ptr<Order>> get(std::string& order_id);

        void enqueue(std::shared_ptr<Order>& order) const;

        OrderRepository(OrderRepository const &) = delete;
        OrderRepository(OrderRepository &&) = delete;
        OrderRepository &operator=(OrderRepository const &) = delete;
        OrderRepository &operator=(OrderRepository &&) = delete;

    private:
        rocksdb::DB* _db;
        rocksdb::ColumnFamilyHandle* _cf;
        std::unique_ptr<RingBuffer<std::shared_ptr<Order>>> _ring_buffer;
        std::jthread _thread;
};
