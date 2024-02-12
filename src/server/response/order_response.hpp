#include <seastar/core/print.hh>
#include <seastar/http/httpd.hh>
#include <seastar/json/json_elements.hh>

#include "../../data_structures/order.hpp"

struct SeastarOrderJson : public seastar::json::json_base {
    seastar::json::json_element<std::string> id;
    seastar::json::json_element<std::string> instrument;
    seastar::json::json_element<std::string> user_id;
    seastar::json::json_element<std::time_t> timestamp;
    seastar::json::json_element<float> qty;
    seastar::json::json_element<float> current_qty;
    seastar::json::json_element<float> price;
    seastar::json::json_element<float> stop_price;
    seastar::json::json_element<uint8_t> side;
    seastar::json::json_element<bool> cancelled;
    seastar::json::json_element<uint64_t> params;
    seastar::json::json_element<uint8_t> type;

    void register_params() {
        add(&id, "id");
        add(&instrument, "instrument");
        add(&user_id, "user_id");
        add(&timestamp, "timestamp");
        add(&qty, "qty");
        add(&current_qty, "current_qty");
        add(&price, "price");
        add(&stop_price, "stop_price");
        add(&side, "side");
        add(&cancelled, "cancelled");
        add(&params, "params");
        add(&type, "type");
    }

    SeastarOrderJson() {
        register_params();
    }

    SeastarOrderJson(const SeastarOrderJson& e) : seastar::json::json_base() {
        register_params();
        id = e.id;
        instrument = e.instrument;
        user_id = e.user_id;
        timestamp = e.timestamp;
        qty = e.qty;
        current_qty = e.current_qty;
        price = e.price;
        stop_price = e.stop_price;
        side = e.side;
        cancelled = e.cancelled;
        params = e.params;
        type = e.type;
    }

    template<class T>
    SeastarOrderJson& operator=(const T& e){
        id = e.get_id();
        instrument = e.get_instrument();
        user_id = e.get_user_id();
        timestamp = e.get_timestamp();
        qty = e.get_qty();
        current_qty = e.get_current_qty();
        price = e.get_price();
        stop_price = e.get_stop_price();
        side = e.get_side_uint8();
        cancelled = e.is_cancelled();
        params = e.get_params_uint64();
        type = e.get_type_uint8();

        return *this;
    }

    SeastarOrderJson& operator=(const SeastarOrderJson& e){
        id = e.id;
        instrument = e.instrument;
        user_id = e.user_id;
        timestamp = e.timestamp;
        qty = e.qty;
        current_qty = e.current_qty;
        price = e.price;
        stop_price = e.stop_price;
        side = e.side;
        cancelled = e.cancelled;
        params = e.params;
        type = e.type;

        return *this;
    }

    SeastarOrderJson(std::shared_ptr<Order>& o) {
        register_params();

        this->id = o->get_id();
        this->instrument = o->get_instrument();
        this->user_id = o->get_user_id();
        this->timestamp = o->get_timestamp();
        this->qty = o->get_initial_qty();
        this->current_qty = o->get_qty();
        this->price = o->get_price();
        this->stop_price = o->get_stop_price();
        this->side = o->get_side_uint8();
        this->cancelled = o->is_cancelled();
        this->params = o->get_params_uint64();
        this->type = o->get_type_uint8();
    }
};


struct SeastarOrderInfoJson : public seastar::json::json_base {
    seastar::json::json_element<std::string> order_id;
    seastar::json::json_element<uint8_t> status;

    void register_params() {
        add(&order_id, "order_id");
        add(&status, "status");
    }

    SeastarOrderInfoJson() {
        register_params();
    }

    SeastarOrderInfoJson(const SeastarOrderInfoJson& e) : seastar::json::json_base() {
        register_params();
        order_id = e.order_id;
        status = e.status;
    }

    SeastarOrderInfoJson& operator=(const SeastarOrderInfoJson& e){
        order_id = e.order_id;
        status = e.status;

        return *this;
    }

    SeastarOrderInfoJson(std::string& order_id, OrderStatus status) {
        register_params();

        this->order_id = order_id;
        this->status = static_cast<uint8_t>(status);
    }
};
