#include "orderbook.hpp"

OrderBook::OrderBook(std::string&& instrument, std::string&& order_db_path, std::string&& trade_db_path): _instrument{instrument}, _order_repository{std::make_unique<OrderRepository>(order_db_path)}, _trade_repository{std::make_unique<TradeRepository>(trade_db_path,1)}{}

std::uint64_t OrderBook::size(){
    return _orders.size();
}

void OrderBook::add_order(std::shared_ptr<Order>& order){
    bool is_buy = order->is_buy();
    if(order->has_param(OrderParams::STOP)){
        // if((is_buy && _market_price < order->get_price()) || (!is_buy && _market_price > order->get_price())){
        //     return;
        // }
        add_stop_order(order, is_buy ? _bid_stop_orders : _ask_stop_orders);
    }else{
        bool matched = match_order(order, is_buy ? _ask_limits : _bid_limits);
        if(matched){
            _order_repository->save(order);
            return;
        }

        if(order->has_param(OrderParams::FOK)) {
		    order->cancel();                                
        }else {
            auto limit = std::make_shared<Limit>(order->get_price());
            add_limit_order(order,limit, is_buy ? _bid_limits : _ask_limits);
        }
    }

    if(!order->is_cancelled()){
        _orders[order->get_id()] = order;
    }

    _order_repository->save(order);
}

void OrderBook::add_limit_order(std::shared_ptr<Order>& order, std::shared_ptr<Limit>& limit, RBTree<std::shared_ptr<Limit>>& limits){
    auto& maybe_orderbook_limit = limits.find(limit);
    if(maybe_orderbook_limit.has_value()){
        auto orderbook_limit = *maybe_orderbook_limit;
        order->set_limit_parent(orderbook_limit);

        if(!orderbook_limit->_head){
            orderbook_limit->_head = order;
            orderbook_limit->_tail = order;
        }else{
            auto pom = orderbook_limit->_tail;
            pom->_next = order;
            order->_prev = pom;
            orderbook_limit->_tail = order;
        }

    }else{
        limits.insert(limit);
        order->set_limit_parent(limit);
        limit->_head = order;
        limit->_tail = order;
    }
}

void OrderBook::modify_order(std::shared_ptr<Order>& order){
    auto orderbook_entry = _orders.find(order->get_id());
    if(orderbook_entry != _orders.end() && !orderbook_entry->second->is_cancelled() && !orderbook_entry->second->is_fullfilled()){
        orderbook_entry->second->cancel();
        if(orderbook_entry->second->has_param(OrderParams::STOP)){
            remove_stop_order(order);
            add_stop_order(order, order->is_buy() ? _bid_stop_orders : _ask_stop_orders);
        }else{
            remove_limit_order(orderbook_entry->second);
            add_limit_order(order,orderbook_entry->second->get_limit_parent(),order->is_buy() ? _bid_limits : _ask_limits);
        }
    }
}

void OrderBook::remove_limit_order(std::shared_ptr<Order>& orderbook_entry){
    auto& parent_limit = orderbook_entry->get_limit_parent();
    parent_limit->decrease_volume(orderbook_entry->get_qty());
    parent_limit->decrease_number_of_orders();

    if(orderbook_entry->_next && orderbook_entry->_prev){
        orderbook_entry->_next->_prev = orderbook_entry->_prev;
        orderbook_entry->_prev->_next = orderbook_entry->_next;
    }else if(orderbook_entry->_prev){
        orderbook_entry->_prev->_next = nullptr;
    }else{
        orderbook_entry->_next->_prev = nullptr;
    }

    if(parent_limit->_head == orderbook_entry && parent_limit->_tail == orderbook_entry){
        parent_limit->_head = parent_limit->_tail = nullptr;
    }else if(parent_limit->_head == orderbook_entry){
        parent_limit->_head = orderbook_entry->_next;
    }else if(parent_limit->_tail == orderbook_entry){
        parent_limit->_tail = orderbook_entry->_prev;
    }

    _orders.erase(orderbook_entry->get_id());
}

//    std::string buyer_id;
    // std::string seller_id;
    // std::string buyer_order_id;
    // std::string seller_order_id;
    // std::string instrument;
    // float volume;
    // float price;
    // std::time_t timestamp;

bool OrderBook::match_order(std::shared_ptr<Order>& order,RBTree<std::shared_ptr<Limit>>& limits){
    bool is_aon = order->has_param(OrderParams::AON);
    auto order_qty = order->get_qty();
    auto order_type = order->get_type();
    float cross_price;
    std::vector<std::shared_ptr<Limit>> filled_limits;
    for(auto& opposite_limit: limits){

        if(is_aon && opposite_limit->get_active_volume() < order_qty){
            continue;
        }

        auto curr_order = opposite_limit->_head;

        while(order_qty> 0 && curr_order){
            auto curr_order_qty = curr_order->get_qty();

            if(order_qty == curr_order_qty){
                order_qty = 0;
                remove_limit_order(curr_order);
            }else if(!is_aon && order_qty > curr_order_qty){
                order_qty -= curr_order_qty;
                remove_limit_order(curr_order);
            }else{
                curr_order->decrease_qty(order_qty);
                opposite_limit->decrease_volume(order_qty);
                order_qty = 0;
                break;
            }

            curr_order = curr_order->_next;
       }

       opposite_limit->_head = curr_order;

        switch (order_type){
            case OrderType::MARKET:
               cross_price = opposite_limit->get_price();
            case OrderType::LIMIT:
                if(order->is_buy()){
                    if(order->get_price() < opposite_limit->get_price()){
                        return false;
                    }else{
                        cross_price = order->get_price();
                    }
                }else{
                    if(order->get_price() > opposite_limit->get_price()){
                        return false;
                    }else{
                        cross_price = opposite_limit->get_price();
                    }
                }  
        }

        if(opposite_limit->empty()){
           filled_limits.push_back(opposite_limit);
        }

       set_market_price(cross_price);
       
       if(order_qty == 0){
            order->fill();
            break;
       }
    }

    for(auto& limit: filled_limits){
        limit->clear();
        limits.remove(limit);
    }

    return order_qty == 0;
}


Spread OrderBook::get_spread(){
    auto& best_bid = _bid_limits.last();
    auto& best_ask = _ask_limits.first();
    float best_ask_price = -1;
    float best_bid_price = -1;
    if(best_ask.has_value()){
        best_ask_price = best_ask.value()->get_price();
    }

    if(best_bid.has_value()){
        best_bid_price = best_bid.value()->get_price();
    }

    return {best_bid_price,best_ask_price, best_bid_price - best_ask_price};
}

void OrderBook::cancel_order(std::string& order_id){
    auto orderbook_entry = _orders.find(order_id);
    if(orderbook_entry != _orders.end()){
        orderbook_entry->second->cancel();
        if(orderbook_entry->second->has_param(OrderParams::STOP)){
            remove_stop_order(orderbook_entry->second);
        }else{
            remove_limit_order(orderbook_entry->second);
        }
    }
}

void OrderBook::add_stop_order(std::shared_ptr<Order>& order, RBTree<std::shared_ptr<Order>>& orders){
    orders.insert(order);
}

void OrderBook::remove_stop_order(std::shared_ptr<Order>& stop_order){
    if(stop_order->is_buy()){
        _bid_stop_orders.remove(stop_order);
    }else{
        _ask_stop_orders.remove(stop_order);
    }

    _orders.erase(stop_order->get_id());
}

std::vector<std::shared_ptr<Order>> OrderBook::get_bids(){
    std::vector<std::shared_ptr<Order>> bid_orders{};

    for(auto& limit : _bid_limits){
        auto order = limit->_head;
        while(order){
            bid_orders.push_back(order);
            order = order->_next;
        }
    }

    return bid_orders;
}

std::vector<std::shared_ptr<Order>> OrderBook::get_asks(){
    std::vector<std::shared_ptr<Order>>ask_orders{};

    for(auto& limit : _ask_limits){
        auto order = limit->_head;
        while(order){
            ask_orders.push_back(order);
            order = order->_next;
        }
    }

    return ask_orders;
}

std::vector<std::shared_ptr<Order>> OrderBook::get_bid_stop_orders(){
    std::vector<std::shared_ptr<Order>> bid_stop_orders{};

    for(auto& order : _bid_stop_orders){
        bid_stop_orders.push_back(order);
    }

    return bid_stop_orders;
}

std::vector<std::shared_ptr<Order>> OrderBook::get_ask_stop_orders(){
    std::vector<std::shared_ptr<Order>>ask_stop_orders{};

    for(auto& order : _ask_stop_orders){
        ask_stop_orders.push_back(order);    
    }

    return ask_stop_orders;
}

void OrderBook::add_bid_stop_orders_below(float price) {
    auto& top_bid_order = _bid_stop_orders.last();

    if (!top_bid_order.has_value() || top_bid_order.value()->get_price() > price) {
        return;
    }

    std::vector<std::shared_ptr<Order>> orders_to_remove;

    for (auto it = _bid_stop_orders.rbegin(); it.valid(); it++) {
        auto& order = *it;
        if (order->get_price() <= price) {
            add_order(order);
            orders_to_remove.push_back(order);
        } else {
            break;
        }
    }

    for (auto& order : orders_to_remove) {
        _bid_stop_orders.remove(order);
    }
}

void OrderBook::add_ask_stop_orders_above(float price) {
    auto& bottom_ask_order = _ask_stop_orders.first();

    if (!bottom_ask_order.has_value() || bottom_ask_order.value()->get_price() < price) {
        return;
    }

    std::vector<std::shared_ptr<Order>> orders_to_remove;

    for (auto it = _ask_stop_orders.begin(); it.valid(); it++) {
        auto& order = *it;
        if (order->get_price() >= price) {
            add_order(order);
            orders_to_remove.push_back(order);
        } else {
            break;
        }
    }

    for (auto& order : orders_to_remove) {
        _ask_stop_orders.remove(order);
    }
}

std::string& OrderBook::get_instrument(){
    return _instrument;
}

float OrderBook::get_market_price(){
    return _market_price;
}

void OrderBook::set_market_price(float price){
    _market_price = price;
}
