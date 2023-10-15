#include<unordered_map>
#include<string>
#include<memory>

#include "../data_structures/limit.hpp"
#include "../data_structures/order.hpp"
#include "../storage/rbtree.hpp"
#include "../repository/order_repository.hpp"
#include "../repository/trade_repository.hpp"
#include "../utils/defer.hpp"

struct Spread{
    float best_bid;
    float best_ask;
    float spread;
};

class OrderBook final{

    public:

        OrderBook(std::string instrument, float market_price, std::shared_ptr<OrderRepository>& order_repository, std::shared_ptr<TradeRepository>& trade_repository);

        Spread get_spread();

        std::vector<std::shared_ptr<Order>> get_bids();

        std::vector<std::shared_ptr<Order>> get_asks();

        std::vector<std::shared_ptr<Order>> get_bid_stop_orders();

        std::vector<std::shared_ptr<Order>> get_ask_stop_orders();

        float get_market_price();

        std::string& get_instrument();

        OrderStatus add_order(std::shared_ptr<Order>&& order);

        OrderStatus cancel_order(std::string&& order_id);

        OrderStatus modify_order(std::shared_ptr<Order>&& order);

        uint64_t size();

    private:
        void add_limit_order(std::shared_ptr<Order>& order, std::shared_ptr<Limit>& limit, RBTree<std::shared_ptr<Limit>>& limits);

        void add_stop_order(std::shared_ptr<Order>& order, RBTree<std::shared_ptr<Order>>& orders);
        
        void remove_limit_order(std::shared_ptr<Order>& order);

        void remove_stop_order(std::shared_ptr<Order>& stop_order);

        bool match_order(std::shared_ptr<Order>& order,RBTree<std::shared_ptr<Limit>>& limits);

        void set_market_price(float price);

        void add_bid_stop_orders_below(float price);

        void add_ask_stop_orders_above(float price);

        void handle_trade(std::shared_ptr<Order>& recieved_order, std::shared_ptr<Order>& orderbook_entry, float volume, float price);

        std::string _instrument;
        float _market_price;
        RBTree<std::shared_ptr<Limit>> _ask_limits{};
        RBTree<std::shared_ptr<Limit>> _bid_limits{};
        RBTree<std::shared_ptr<Order>> _ask_stop_orders{};
        RBTree<std::shared_ptr<Order>> _bid_stop_orders{};
        std::unordered_map<std::string, std::shared_ptr<Order>> _orders{};

        std::shared_ptr<OrderRepository> _order_repository;
        std::shared_ptr<TradeRepository> _trade_repository;
};
