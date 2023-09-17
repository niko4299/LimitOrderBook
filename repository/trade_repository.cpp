#include "trade_repository.hpp"


TradeRepository::TradeRepository(std::string&& hosts, uint32_t batch_size, std::size_t ringbuffer_size) {
    _cluster = cass_cluster_new();
    _session = cass_session_new();

    cass_cluster_set_contact_points(_cluster, hosts.c_str());
    auto connect_future = std::unique_ptr<CassFuture, decltype(&cass_future_free)>(cass_session_connect(_session, _cluster), &cass_future_free);

    if (cass_future_error_code(connect_future.get()) != CASS_OK){
      throw std::runtime_error("Failed connecting to scylla db.");
    }

    const char* insert_query = "INSERT INTO orderbook.trades (buyer_id, seller_id, buyer_order_id, seller_order_id, instrument, timestamp, volume, price) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    const char* select_by_primary_key_query = "SELECT * FROM orderbook.trades where buyer_id=? and seller_id=? and timestamp=?;";
    CassFuture* prep_future = cass_session_prepare(_session, insert_query);
    cass_future_wait(prep_future);
    _prepared_insert_query = cass_future_get_prepared(prep_future);
    prep_future = cass_session_prepare(_session, select_by_primary_key_query);
    cass_future_wait(prep_future);
    _prepared_select_by_primary_key_query = std::move(cass_future_get_prepared(prep_future));
    cass_future_free(prep_future);

    _batch_size = batch_size;
    _ring_buffer = std::make_unique<RingBuffer<Trade>>(ringbuffer_size);
}

void TradeRepository::enqueue(Trade& trade) const {
    _ring_buffer->push(trade);
}

void TradeRepository::process_message(std::stop_token& s){
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
    cass_statement_bind_int64(bound_statement, 5,  cass_time_from_epoch(trade.timestamp));
    cass_statement_bind_float(bound_statement, 6, trade.volume);
    cass_statement_bind_float(bound_statement, 7, trade.price);

    CassFuture* exec_future = cass_session_execute(_session, bound_statement);
    cass_future_wait(exec_future);

    bool ok = cass_future_error_code(exec_future) == CASS_OK;

    cass_future_free(exec_future);
    cass_statement_free(bound_statement);

    return ok;
}

const char* get_column_string(const CassRow* row,std::string&& column_name){
    const CassValue* value = cass_row_get_column_by_name(row, column_name.c_str());
    const char* column_value;
    size_t length;
    cass_value_get_string(value, &column_value, &length);

    return std::move(column_value);
}

float get_column_float(const CassRow* row,std::string&& column_name){
    const CassValue* value = cass_row_get_column_by_name(row, column_name.c_str());
    float column_value;
    cass_value_get_float(value, &column_value);

    return std::move(column_value);
}

std::int64_t get_column_int64(const CassRow* row,std::string&& column_name){
    const CassValue* value = cass_row_get_column_by_name(row, column_name.c_str());
    cass_int64_t column_value;
    cass_value_get_int64(value, &column_value);

    return std::move(column_value);
}

Trade convert_to_trade(CassIterator*& iterator){
    Trade trade;
    const CassRow* row = cass_iterator_get_row(iterator);

    trade.buyer_id = get_column_string(row, std::move("buyer_id"));
    trade.seller_id = get_column_string(row, std::move("seller_id"));
    trade.buyer_order_id = get_column_string(row, std::move("buyer_order_id"));
    trade.seller_order_id = get_column_string(row, std::move("seller_order_id"));
    trade.instrument = get_column_string(row, std::move("instrument"));
    trade.volume = get_column_float(row, std::move("volume"));
    trade.price = get_column_float(row, std::move("price"));     
    trade.timestamp = std::time_t(get_column_int64(row, std::move("timestamp")));

    return trade;
}

std::vector<Trade> TradeRepository::get_all_trades(){
    CassStatement* statement = cass_statement_new(_query_all, 0);
    CassFuture* result_future = cass_session_execute(_session, statement);
    std::vector<Trade> all_trades{};
    
    if (cass_future_error_code(result_future) == CASS_OK) {
        const CassResult* result = cass_future_get_result(result_future);
        CassIterator* iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            all_trades.push_back(convert_to_trade(iterator));
        }

        cass_result_free(result);
        cass_iterator_free(iterator);
    }

    cass_statement_free(statement);
    cass_future_free(result_future);
    
    return all_trades;
}

std::optional<Trade> TradeRepository::get_trade_by_primary_key(std::string&& buyer_id, std::string&& seller_id, std::time_t timestamp){
    auto statement = cass_prepared_bind(_prepared_select_by_primary_key_query);

    cass_statement_bind_string(statement, 0, buyer_id.c_str());
    cass_statement_bind_string(statement, 1, seller_id.c_str());
    cass_statement_bind_int64(statement, 2, cass_time_from_epoch(timestamp));

    auto future = cass_session_execute(_session, statement);
    cass_future_wait(future);

    if (cass_future_error_code(future) != CASS_OK) {
        return std::nullopt;
    } 

    const CassResult* result = cass_future_get_result(future);
    CassIterator* iterator = cass_iterator_from_result(result);
    cass_iterator_next(iterator);
    auto trade = convert_to_trade(iterator);

    cass_result_free(result);
    cass_iterator_free(iterator);
    cass_future_free(future);
    cass_statement_free(statement);

    return std::move(trade);
}

bool TradeRepository::run_query(std::string&& query){
    CassStatement* statement = cass_statement_new(query.c_str(), 0);

    CassFuture* result_future = cass_session_execute(_session, statement);
    bool ok = cass_future_error_code(result_future) == CASS_OK;

    cass_statement_free(statement);
    cass_future_free(result_future);
    return ok;
}

