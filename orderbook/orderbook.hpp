#include<unordered_map>
#include<string>
#include<memory>

#include "../data_structures/limit.hpp"
#include "../data_structures/order.hpp"
#include "../storage/rbtree.hpp"
#include "../repository/order_repository.hpp"
#include "../repository/trade_repository.hpp"


struct Spread{
    float best_bid;
    float best_ask;
    float spread;
};

class OrderBook final{

    public:

        OrderBook(std::string&& instrument, std::string&& order_db_path, std::string&& trade_db_path);

        Spread get_spread();

        std::vector<std::shared_ptr<Order>> get_bids();

        std::vector<std::shared_ptr<Order>> get_asks();

        std::vector<std::shared_ptr<Order>> get_bid_stop_orders();

        std::vector<std::shared_ptr<Order>> get_ask_stop_orders();

        float get_market_price();

        std::string& get_instrument();

        void add_order(std::shared_ptr<Order>& order);

        void cancel_order(std::string& order_id);

        void modify_order(std::shared_ptr<Order>& order);

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


        float _market_price;
        std::string _instrument;
        RBTree<std::shared_ptr<Limit>> _ask_limits{};
        RBTree<std::shared_ptr<Limit>> _bid_limits{};
        RBTree<std::shared_ptr<Order>> _ask_stop_orders{};
        RBTree<std::shared_ptr<Order>> _bid_stop_orders{};
        std::unordered_map<std::string, std::shared_ptr<Order>> _orders{};

        std::unique_ptr<OrderRepository> _order_repository;
        std::unique_ptr<TradeRepository> _trade_repository;
};