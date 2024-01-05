#include "order_repository.hpp"

OrderRepository::OrderRepository(std::string&& db_path, std::size_t ringbuffer_size) {
    auto file_system_db_path = std::filesystem::path(db_path);
    if (!std::filesystem::is_directory(file_system_db_path)) {
        auto ok = std::filesystem::create_directory(file_system_db_path);
        if (!ok) {
            throw std::runtime_error("Couldn't create directory with path: " + db_path);
        }
    }

    rocksdb::Options options{};
    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();
    options.create_if_missing = true;

    auto status = rocksdb::DB::Open(options, db_path, &_db);
    if (!status.ok()) {
        throw std::runtime_error("Couldn't open connection with order repository database");
    }

    status = _db->CreateColumnFamily(rocksdb::ColumnFamilyOptions(), "order_cf", &_cf);
    if (!status.ok()) {
        throw std::runtime_error("Couldn't create column family handle with order repository database");
    }

    _ring_buffer = std::make_unique<RingBuffer<std::shared_ptr<Order>>>(ringbuffer_size);
    _thread = std::jthread(std::bind_front(&OrderRepository::process_messages, this));
}

OrderRepository::~OrderRepository() {
    // TODO: decied what to do with this status returns.
    auto status = _db->DropColumnFamily(_cf);
    status = _db->DestroyColumnFamilyHandle(_cf);
    _cf = nullptr;

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

    auto status = _db->Put(rocksdb::WriteOptions(),_cf, order->get_id(), oss.str());

    return status.ok();
}

std::optional<std::shared_ptr<Order>> OrderRepository::get(std::string& order_id) {
    std::string order_serialized;
    auto status = _db->Get(rocksdb::ReadOptions(),_cf, order_id, &order_serialized);
    if (status.ok()) {
        std::shared_ptr<Order> ret_order;
        std::istringstream iss(order_serialized);
        boost::archive::text_iarchive ia(iss);
        ia >> ret_order;

        return ret_order;
    }

    return std::nullopt;
}
