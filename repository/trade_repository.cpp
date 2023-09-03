#include "trade_repository.hpp"


TradeRepository::TradeRepository(std::string& db_path, uint32_t batch_size, std::shared_ptr<boost::lockfree::spsc_queue<Trade, boost::lockfree::capacity<1024UL>>>& ring_buffer) {
    _cluster = cass_cluster_new();
    _session = cass_session_new();
    auto connect_future = std::unique_ptr<CassFuture, decltype(&cass_future_free)>(cass_session_connect(_session, _cluster), &cass_future_free);

    if (cass_future_error_code(connect_future.get()) != CASS_OK){
      throw std::runtime_error("Failed connecting to scylla db.");
    }else{
        const char* insert_query = "INSERT INTO orderbook.trades (buyer_id, seller_id, buyer_order_id, seller_order_id, instrument, timestamp, volume, price) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
        CassFuture* prep_future = cass_session_prepare(_session, insert_query);
        cass_future_wait(prep_future);
        _prepared_insert_query = cass_future_get_prepared(prep_future);
        cass_future_free(prep_future);
    }

    _batch_size = batch_size;
    _ring_buffer = ring_buffer;
}

void TradeRepository::process_message(std::stop_token s){
    Trade trade;
    while(!s.stop_requested()){
        while(_ring_buffer->pop(trade)){
            save(trade);
        }
    } 
}

TradeRepository::~TradeRepository() {
  cass_cluster_free(_cluster);
  cass_session_free(_session);
}

bool TradeRepository::save(Trade trade) {
    CassStatement* bound_statement = cass_prepared_bind(_prepared_insert_query);
    cass_statement_bind_string(bound_statement, 0, trade.buyer_id.c_str());
    cass_statement_bind_string(bound_statement, 1, trade.seller_id.c_str());
    cass_statement_bind_string(bound_statement, 2, trade.buyer_order_id.c_str());
    cass_statement_bind_string(bound_statement, 3, trade.seller_order_id.c_str());
    cass_statement_bind_string(bound_statement, 4, trade.instrument.c_str());
    cass_statement_bind_int64(bound_statement, 5, trade.timestamp);
    cass_statement_bind_float(bound_statement, 6, trade.volume);
    cass_statement_bind_float(bound_statement, 7, trade.price);

    CassFuture* exec_future = cass_session_execute(_session, bound_statement);
    cass_future_wait(exec_future);

    bool ok = cass_future_error_code(exec_future) == CASS_OK;

    cass_future_free(exec_future);
    cass_statement_free(bound_statement);

    return ok;
}
