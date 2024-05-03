#pragma once

#include<unordered_map>
#include<string>
#include<memory>

#include "../data_structures/limit.hpp"
#include "../data_structures/order.hpp"
#include "../data_structures/spread.hpp"
#include "../data_structures/snapshot.hpp"
#include "../storage/rbtree.hpp"
#include "../repository/order_repository.hpp"
#include "../repository/trade_repository.hpp"
#include "../utils/defer.hpp"

class OrderBook final{

    public:

        OrderBook(std::string_view instrument, float market_price, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository);

        ~OrderBook() = default;
        
        Spread get_spread();

        std::vector<std::shared_ptr<Order>> get_bids();

        std::vector<std::shared_ptr<Order>> get_asks();

        std::vector<std::shared_ptr<Order>> get_bid_stop_orders();

        std::vector<std::shared_ptr<Order>> get_ask_stop_orders();

        // TODO: refactor snapshots
        Snapshot get_snapshot();

        float get_market_price();

        std::string_view get_instrument();

        OrderStatus add_order(std::shared_ptr<Order>&& order);

        OrderStatus cancel_order(std::string_view order_id);

        OrderStatus modify_order(std::shared_ptr<Order>&& order);

        void check_date_orders();

        uint64_t size();

    private:
        void add_limit_order(std::shared_ptr<Order>& order, std::shared_ptr<Limit>& limit, RBTree<std::shared_ptr<Limit>, LimitComparator>& limits);

        void add_stop_order(std::shared_ptr<Order>& order, RBTree<std::shared_ptr<Order>,OrderComparator>& orders);
        
        void remove_limit_order(std::shared_ptr<Order>& order);

        void remove_stop_order(std::shared_ptr<Order>& stop_order);

        bool match_order(std::shared_ptr<Order>& order,RBTree<std::shared_ptr<Limit>, LimitComparator>& limits);

        void set_market_price(float price);

        void add_bid_stop_orders_below(float price);

        void add_ask_stop_orders_above(float price);

        void handle_trade(std::shared_ptr<Order>& recieved_order, std::shared_ptr<Order>& orderbook_entry, float volume, float price);
                
        std::optional<float> calculate_cross_price(const std::shared_ptr<Order>& order,
                                           const std::shared_ptr<Limit>& opposite_limit,
                                           OrderType order_type, 
                                           bool is_buy);

        
        void process_limit_order(std::shared_ptr<Limit>& opposite_limit,
                                            std::shared_ptr<Order>& order, float cross_price, 
                                            std::vector<std::shared_ptr<Order>>& changed_orders);


        void resolve_orders(std::shared_ptr<Order>& curr_order,
                         std::shared_ptr<Order>& order, float cross_price,
                         std::vector<std::shared_ptr<Order>>& changed_orders);

        std::vector<SnapshotLimit> get_snapshot_bids();

        std::vector<SnapshotLimit> get_snapshot_asks();

        std::string_view _instrument;
        float _market_price;
        RBTree<std::shared_ptr<Limit>, LimitComparator> _ask_limits{LimitComparator{true}};
        RBTree<std::shared_ptr<Limit>, LimitComparator> _bid_limits{LimitComparator{false}};
        RBTree<std::shared_ptr<Order>, OrderComparator> _ask_stop_orders{OrderComparator{false}};
        RBTree<std::shared_ptr<Order>, OrderComparator> _bid_stop_orders{OrderComparator{true}};
        RBTree<std::shared_ptr<Order>, DateOrderComparator> _date_orders{DateOrderComparator{true}};
        std::unordered_map<std::string_view, std::shared_ptr<Order>> _orders{};

        std::shared_ptr<OrderRepository> _order_repository;
        std::shared_ptr<TradeRepository> _trade_repository;
};
