#include<unordered_map>
#include<string>
#include<memory>

#include "../orderbook/orderbook.hpp"

class Exchange final {

    public:
        explicit Exchange();

        ~Exchange();

        void add_order();

        void modify_order();

        void cancel_order();

        void add_instrument();

    private:
        std::unordered_map<std::string, std::shared_ptr<OrderBook>> _instruments;
};