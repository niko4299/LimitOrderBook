#include "order_repository.hpp"

OrderRepository::OrderRepository(std::string&& db_path, std::size_t ringbuffer_size) {
    auto file_system_db_path = std::filesystem::path(db_path);
    _options = rocksdb::Options{};
    _options.IncreaseParallelism();
    _options.OptimizeLevelStyleCompaction();
    _options.create_missing_column_families = false;
    _options.create_if_missing = true;

    if (!std::filesystem::exists(file_system_db_path)) {
        auto ok = std::filesystem::create_directory(file_system_db_path);
        if (!ok) {
            throw std::runtime_error("Couldn't create directory with path: " + db_path);
        }

        auto status = rocksdb::TransactionDB::Open(_options, rocksdb::TransactionDBOptions(), db_path, &_db);
        if (!status.ok()) {
            throw std::runtime_error("Couldn't open connection with order repository database");
        }

        status = _db->CreateColumnFamily(_options, OrderColumnFamily, &_order_handler);
        if (!status.ok()) {
            throw std::runtime_error("Couldn't create column family handle with order repository database");
        }
    } else {
        std::vector<rocksdb::ColumnFamilyHandle *> column_handles;
        std::vector<rocksdb::ColumnFamilyDescriptor> column_families;
        
        column_families.emplace_back(OrderColumnFamily, _options);
        column_families.emplace_back(DefaultColumnFamily, _options);

        rocksdb::TransactionDB::Open(_options, rocksdb::TransactionDBOptions(), db_path, column_families, &column_handles, &_db);

        _order_handler = column_handles[0];
        _default_handler = column_handles[1];
    }

    _ring_buffer = std::make_unique<RingBuffer<std::shared_ptr<Order>>>(ringbuffer_size);
    _thread = std::jthread(std::bind_front(&OrderRepository::process_messages, this));
}

OrderRepository::~OrderRepository() {
    // TODO: decied what to do with this status returns.
    _thread.request_stop();
    _thread.join();
    auto status = _db->DestroyColumnFamilyHandle(_order_handler);
    if (_default_handler) {
        status = _db->DestroyColumnFamilyHandle(_default_handler);
        _default_handler = nullptr;
    }
    _db->Close();

    _order_handler = nullptr;

    delete _db;
    _db = nullptr;
}

void OrderRepository::enqueue(std::shared_ptr<Order>& order) const {
    _ring_buffer->push(order);
}

void OrderRepository::process_messages(std::stop_token s){
    std::shared_ptr<Order> order;
    while(!s.stop_requested()){
        if(_ring_buffer->pop(order)){
            save(order);
        }
    } 
}

bool OrderRepository::save(std::shared_ptr<Order>& order) {
    std::ostringstream oss;
    boost::archive::text_oarchive oa(oss);
    oa << order;

    auto status = _db->Put(rocksdb::WriteOptions(), _order_handler, order->get_id(), oss.str());

    return status.ok();
}

std::optional<std::shared_ptr<Order>> OrderRepository::get(std::string_view order_id) {
    std::string order_serialized;
    auto status = _db->Get(rocksdb::ReadOptions(), _order_handler , order_id, &order_serialized);
    if (status.ok()) {
        std::shared_ptr<Order> ret_order;
        std::istringstream iss(order_serialized);
        boost::archive::text_iarchive ia(iss);
        ia >> ret_order;

        return ret_order;
    }

    return std::nullopt;
}
