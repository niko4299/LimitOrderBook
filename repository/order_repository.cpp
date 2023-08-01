#include "order_repository.hpp"

OrderRepository::OrderRepository(std::string& db_path, std::shared_ptr<boost::lockfree::spsc_queue<std::shared_ptr<Order>, boost::lockfree::capacity<1024>>>& ring_buffer) {
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
    _ring_buffer = ring_buffer;
}

OrderRepository::~OrderRepository() {
    _done = true;
    delete _db;
}

void OrderRepository::process_message(){
    std::shared_ptr<Order> order;
    while(!_done){
        while(_ring_buffer->pop(order)){
            save(order);
        }
    } 
}

bool OrderRepository::save(std::shared_ptr<Order>& order) {
    std::ostringstream oss;
    boost::archive::text_oarchive oa(oss);
    oa << order;

    auto status = _db->Put(rocksdb::WriteOptions(), order->get_id(), oss.str());

    return status.ok();
}

std::optional<std::shared_ptr<Order>> OrderRepository::get(std::string& order_id) {
    std::string order_serialized;
    auto status = _db->Get(rocksdb::ReadOptions(), order_id, &order_serialized);
    if (status.ok()) {
        std::shared_ptr<Order> ret_order;
        std::istringstream iss(order_serialized);
        boost::archive::text_iarchive ia(iss);
        ia >> ret_order;

        return ret_order;
    }

    return std::nullopt;
}
