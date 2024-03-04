
#include<simdjson.h>

#include "../data_structures/order.hpp"
#include "../data_structures/enums.hpp"

class OrderJsonOrderMapper {
    public: 
        explicit OrderJsonOrderMapper() = default;
        
        std::shared_ptr<Order> map_json_to_order(simdjson::ondemand::document& order_json){
            std::string_view order_id;
            auto error = order_json["order_id"].get(order_id);
            if (error){
                order_id = "";
            }
            
            std::string_view instrument = order_json["instrument"];
            std::string_view user_id = order_json["user_id"];
            double qty = order_json["qty"];
            double price = order_json["price"];
            Side order_side = get_order_side(order_json["side"]);
            OrderType type = get_order_type(order_json["type"]);
            OrderParams params = get_order_params(order_json["params"]);
            if (params == OrderParams::STOP){
                double stop_price = order_json["stop_price"];

                return std::make_shared<Order>(order_id, instrument, user_id,  static_cast<float>(qty), static_cast<float>(price), static_cast<float>(stop_price), order_side, params, type);    
            }

            return std::make_shared<Order>(order_id, instrument, user_id, static_cast<float>(qty), static_cast<float>(price), order_side, params, type);    
        }

    private:
        Side get_order_side(std::string_view side){
            if (side == "BUY"){
                return Side::BUY;
            }else if (side == "SELL"){
                return Side::SELL;
            }

            return Side::UNKOWN;
        }

        OrderType get_order_type(std::string_view type){
            return type == "MARKET" ? OrderType::MARKET : OrderType::LIMIT;
        }

        OrderParams get_order_params(uint64_t params){
            return static_cast<OrderParams>(params);
        }
};
