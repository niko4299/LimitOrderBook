#include "orderbook.hpp"

OrderBook::OrderBook(std::string_view instrument, float market_price, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository)
    : _instrument{instrument},
     _market_price{market_price},
     _order_repository{order_repository},
     _trade_repository{trade_repository} {}


std::uint64_t OrderBook::size() {
    return _orders.size();
}

OrderStatus OrderBook::add_order(std::shared_ptr<Order>&& order) {
    Defer([&](){_order_repository->enqueue(order);});

    bool is_buy = order->is_buy();

    if (order->has_param(OrderParams::STOP)) {

        // If buy and market price is gte or is sell and market price is lte then add stop order
        // otherwise process immediately.
        if ((is_buy && _market_price > order->get_stop_price()) || (!is_buy && _market_price < order->get_stop_price())) {
            add_stop_order(order, is_buy ? _bid_stop_orders : _ask_stop_orders);
            
            return OrderStatus::ACCEPTED;
        }

        if (order->get_type() == OrderType::MARKET) {
            order->set_params(OrderParams::IOC);
        }
    }

    if (match_order(order, is_buy ? _ask_limits : _bid_limits)) {
        return OrderStatus::MATCHED;
    }

    if (order->has_param(OrderParams::FOK)) {
        order->cancel();
        return OrderStatus::NOT_MATCHED;
    } 

    auto limit = std::make_shared<Limit>(order->get_price());
    add_limit_order(order, limit, is_buy ? _bid_limits : _ask_limits);
    _orders[order->get_id()] = order;

    return OrderStatus::ACCEPTED;   
}

void OrderBook::add_limit_order(std::shared_ptr<Order>& order, std::shared_ptr<Limit>& limit, RBTree<std::shared_ptr<Limit>>& limits) {
    auto& maybe_orderbook_limit = limits.find(limit);
    if (maybe_orderbook_limit.has_value()) {
        auto orderbook_limit = *maybe_orderbook_limit;
        order->set_limit_parent(orderbook_limit);

        if (!orderbook_limit->_head) {
            orderbook_limit->_head = order;
            orderbook_limit->_tail = order;
        } else {
            auto pom = orderbook_limit->_tail;
            pom->_next = order;
            order->_prev = pom;
            orderbook_limit->_tail = order;
        }
        orderbook_limit->increase_volume(order->get_qty());

    } else {
        limits.insert(limit);
        order->set_limit_parent(limit);
        limit->_head = order;
        limit->_tail = order;
        limit->increase_volume(order->get_qty());
    }
}

OrderStatus OrderBook::modify_order(std::shared_ptr<Order>&& order) {
    auto orderbook_entry = _orders.find(order->get_id());
    if (orderbook_entry != _orders.end() && !orderbook_entry->second->is_cancelled() && !orderbook_entry->second->is_fullfilled()) {
        orderbook_entry->second->cancel();
        if (orderbook_entry->second->has_param(OrderParams::STOP)) {
            remove_stop_order(order);
            add_stop_order(order, order->is_buy() ? _bid_stop_orders : _ask_stop_orders);
        } else {
            remove_limit_order(orderbook_entry->second);
            add_limit_order(order, orderbook_entry->second->get_limit_parent(), order->is_buy() ? _bid_limits : _ask_limits);
        }
        _order_repository->enqueue(order);

        return OrderStatus::MODIFIED;
    }

    return OrderStatus::NOT_FOUND;
}

void OrderBook::remove_limit_order(std::shared_ptr<Order>& orderbook_entry) {
    auto& parent_limit = orderbook_entry->get_limit_parent();
    parent_limit->decrease_volume(orderbook_entry->get_qty());
    parent_limit->decrease_number_of_orders();

    if (orderbook_entry->_next && orderbook_entry->_prev) {
        orderbook_entry->_next->_prev = orderbook_entry->_prev;
        orderbook_entry->_prev->_next = orderbook_entry->_next;
    } else if (orderbook_entry->_prev) {
        orderbook_entry->_prev->_next = nullptr;
    } else if (orderbook_entry->_next) {
        orderbook_entry->_next->_prev = nullptr;
    }

    if (parent_limit->_head == orderbook_entry && parent_limit->_tail == orderbook_entry) {
        parent_limit->_head = parent_limit->_tail = nullptr;
    } else if (parent_limit->_head == orderbook_entry) {
        parent_limit->_head = orderbook_entry->_next;
    } else if (parent_limit->_tail == orderbook_entry) {
        parent_limit->_tail = orderbook_entry->_prev;
    }

    _orders.erase(orderbook_entry->get_id());
}

bool OrderBook::match_order(std::shared_ptr<Order>& order, RBTree<std::shared_ptr<Limit>>& limits) {
    const bool is_aon = order->has_param(OrderParams::AON);
    const bool is_buy = order->is_buy();
    const auto order_type = order->get_type();
    std::vector<std::shared_ptr<Limit>> filled_limits;
    std::vector<std::shared_ptr<Order>> changed_orders;
    Defer([&](){    
        for (auto& limit : filled_limits) {
            limits.remove(limit);
        };

        for(auto& order: changed_orders){         
         _order_repository->enqueue(order);
        }
    }
    );

    for (auto it = limits.begin(); it.valid(); it++) {
        auto& opposite_limit = *it;

        if (is_aon && opposite_limit->get_active_volume() < order->get_qty()) {
            continue;
        }

        auto cross_price = calculate_cross_price(order, opposite_limit, order_type, is_buy);
        if(!cross_price.has_value()){
            break;
        }

        process_limit_order(opposite_limit, order, *cross_price, changed_orders);
        if (opposite_limit->empty()) {
            filled_limits.push_back(opposite_limit);
        }

        set_market_price(*cross_price);
        if (order->get_qty() == 0) {
            return true;
        }
    }

    return false;
}

std::optional<float> OrderBook::calculate_cross_price(const std::shared_ptr<Order>& order,
                                                    const std::shared_ptr<Limit>& opposite_limit,
                                                    OrderType order_type,
                                                    bool is_buy) {
    if ((is_buy && order->get_price() < opposite_limit->get_price()) ||
               (!is_buy && order->get_price() > opposite_limit->get_price())) {
        return std::nullopt;
    } else if (order_type == OrderType::MARKET) {
        return opposite_limit->get_price();
    } else if (order_type == OrderType::LIMIT){
        return is_buy ? order->get_price() : opposite_limit->get_price();
    }

    return std::nullopt;
}

void OrderBook::process_limit_order(std::shared_ptr<Limit>& opposite_limit, std::shared_ptr<Order>& order, float cross_price, std::vector<std::shared_ptr<Order>>& changed_orders) {
    auto curr_order = opposite_limit->_head;
    while (order->get_qty() > 0 && curr_order) {
        resolve_orders(curr_order, order, cross_price, changed_orders);
        if (curr_order->is_fullfilled()) {
            remove_limit_order(curr_order);
        }

        curr_order = curr_order->_next;
    }
}

void OrderBook::resolve_orders(std::shared_ptr<Order>& curr_order,
                               std::shared_ptr<Order>& order, float cross_price,
                               std::vector<std::shared_ptr<Order>>& changed_orders) {
    auto curr_order_qty = curr_order->get_qty();
    auto order_qty = order->get_qty();
    auto before_trade = order_qty;

    if (curr_order_qty >= order_qty) {
        curr_order->decrease_qty(order_qty);
        order->fill();
    } else if (!order->has_param(OrderParams::AON)) {
        order->decrease_qty(curr_order_qty);
        curr_order->fill();
    } 

    if (order_qty != before_trade) {
        handle_trade(order, curr_order, before_trade - order_qty, cross_price);
        changed_orders.push_back(curr_order);
    }
}

Spread OrderBook::get_spread() {
    auto& best_bid = _bid_limits.last();
    auto& best_ask = _ask_limits.first();
    float best_ask_price = -1;
    float best_bid_price = -1;
    if (best_ask.has_value()) {
        best_ask_price = best_ask.value()->get_price();
    }

    if (best_bid.has_value()) {
        best_bid_price = best_bid.value()->get_price();
    }

    return {best_bid_price, best_ask_price, best_ask_price - best_bid_price};
}

OrderStatus OrderBook::cancel_order(std::string_view order_id) {
    auto orderbook_entry = _orders.find(order_id);
    if (orderbook_entry != _orders.end()) {
        orderbook_entry->second->cancel();
        if (orderbook_entry->second->has_param(OrderParams::STOP)) {
            remove_stop_order(orderbook_entry->second);
        } else {
            remove_limit_order(orderbook_entry->second);
        }

        _order_repository->enqueue(orderbook_entry->second);

        return OrderStatus::CANCELLED;
    }

    return OrderStatus::NOT_FOUND;
}

void OrderBook::add_stop_order(std::shared_ptr<Order>& order, RBTree<std::shared_ptr<Order>>& orders) {
    orders.insert(order);
}

void OrderBook::remove_stop_order(std::shared_ptr<Order>& stop_order) {
    if (stop_order->is_buy()) {
        _bid_stop_orders.remove(stop_order);
    } else {
        _ask_stop_orders.remove(stop_order);
    }

    _orders.erase(stop_order->get_id());
}

std::vector<std::shared_ptr<Order>> OrderBook::get_bids() {
    std::vector<std::shared_ptr<Order>> bid_orders{};

    for (auto it = _bid_limits.begin(); it.valid(); it++) {
        auto order = (*it)->_head;
        while (order) {
            bid_orders.push_back(order);
            order = order->_next;
        }
    }

    return bid_orders;
}

std::vector<std::shared_ptr<Order>> OrderBook::get_asks() {
    std::vector<std::shared_ptr<Order>> ask_orders{};

    for (auto it = _ask_limits.begin(); it.valid(); it++) {
        auto order = (*it)->_head;
        while (order) {
            ask_orders.push_back(order);
            order = order->_next;
        }
    }

    return ask_orders;
}

std::vector<std::shared_ptr<Order>> OrderBook::get_bid_stop_orders() {
    std::vector<std::shared_ptr<Order>> bid_stop_orders{};

    for (auto it = _bid_stop_orders.begin(); it.valid(); it++) {
        bid_stop_orders.push_back(*it);
    }

    return bid_stop_orders;
}

std::vector<std::shared_ptr<Order>> OrderBook::get_ask_stop_orders() {
    std::vector<std::shared_ptr<Order>> ask_stop_orders{};

    for (auto it = _ask_stop_orders.begin(); it.valid(); it++) {
        ask_stop_orders.push_back(*it);
    }

    return ask_stop_orders;
}

void OrderBook::add_bid_stop_orders_below(float price) {
    std::vector<std::shared_ptr<Order>> orders_to_remove;

    for (auto it = _bid_stop_orders.rbegin(); it.valid(); it++) {
        auto& order = *it;
        if (order->get_stop_price() <= price) {
            add_order(std::move(order));
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
    std::vector<std::shared_ptr<Order>> orders_to_remove;

    for (auto it = _ask_stop_orders.begin(); it.valid(); it++) {
        auto& order = *it;
        if (order->get_stop_price() >= price) {
            add_order(std::move(order));
            orders_to_remove.push_back(order);
        } else {
            break;
        }
    }

    for (auto& order : orders_to_remove) {
        _ask_stop_orders.remove(order);
    }
}

Snapshot OrderBook::get_snapshot(){
    return Snapshot{get_asks(), get_bids(), get_spread()};
}

std::string_view OrderBook::get_instrument() {
    return _instrument;
}

float OrderBook::get_market_price() {
    return _market_price;
}

void OrderBook::set_market_price(float price) {
    _market_price = price;
}

void OrderBook::handle_trade(std::shared_ptr<Order>& recieved_order, std::shared_ptr<Order>& orderbook_entry, float volume, float price){
        Trade trade;
        
        trade.timestamp = time(0);
        trade.instrument = _instrument;
        if(recieved_order->is_buy()){
            trade.buyer_id = recieved_order->get_user_id();
            trade.buyer_order_id = recieved_order->get_id();
            trade.seller_id = orderbook_entry->get_user_id();
            trade.seller_order_id = orderbook_entry->get_id();
        }else{
            trade.buyer_id = orderbook_entry->get_user_id();
            trade.buyer_order_id = orderbook_entry->get_id();   
            trade.seller_id = recieved_order->get_user_id();
            trade.seller_order_id = recieved_order->get_id();
        }
        trade.volume = volume;
        trade.price = price;
        
        _trade_repository->enqueue(trade);
}
