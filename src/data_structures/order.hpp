#ifndef Order_HPP
#define Order_HPP

#include <cstdint>
#include <string>
#include <ctime>
#include <stdexcept>
#include <memory>
#include <vector>
#include <fstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "enums.hpp"

class Limit;

class Order final {
    public:
        Order() = default;

        Order(std::string_view id, std::string_view instrument, std::string_view user_id, float qty, float price, Side side, OrderParams params, OrderType type, time_t expire_time);
        Order(std::string_view instrument, std::string_view user_id, float qty, float price, Side side, OrderParams params, OrderType type, time_t expire_time);
        Order(std::string_view id, std::string_view instrument, std::string_view user_id, float qty, float price, float stop_price, Side side, OrderParams params, OrderType type, time_t expire_time);
        Order(std::string_view instrument, std::string_view user_id, float qty, float price, float stop_price, Side side, OrderParams params, OrderType type, time_t expire_time);

        ~Order() = default;
        
        Order(Order const &) = delete;
        Order(Order &&) = delete;
        Order &operator=(Order const &) = delete;
        Order &operator=(Order &&) = delete;

        void set_id(std::string_view id);

        std::string& get_id();

        float get_price();

        float get_stop_price();

        std::time_t get_timestamp() const;

        float get_initial_qty() const;

        std::uint8_t get_type_uint8();

        std::uint8_t get_side_uint8();

        std::uint64_t get_params_uint64();

        Side get_side();

        void set_price(float price);

        bool is_buy();

        void increase_qty(float delta_qty);

        void decrease_qty(float delta_qty);

        void cancel();

        bool is_cancelled();

        float get_qty();
        
        void fill();
        
        bool is_fullfilled();

        OrderType get_type();

        std::string& get_user_id();

        void set_params(OrderParams param);

        bool has_param(OrderParams param) const;

        void set_limit_parent(std::shared_ptr<Limit>& limit);

        std::shared_ptr<Limit>& get_limit_parent();

        std::string& get_instrument();

        void set_expire_time(uint64_t time_unix);

        std::time_t get_expire_time();

        
        bool operator==(const Order& other);

        bool operator!=(const Order& other);
        
    public:
        std::shared_ptr<Order> _prev{};
        std::shared_ptr<Order> _next{};

    private:
            
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &a, const unsigned version){
          a & _id & _instrument & _user_id &  _timestamp & _qty & _current_qty & _price & _cancelled & _side & _type & _params & _expire_time;
        }


        std::shared_ptr<Limit> _parent{};
        std::string _id;
        std::string _instrument;
        std::string _user_id;
        std::time_t _timestamp;
        float _qty;
        float _current_qty;
        float _price;
        // used only in stop orders.
        float _stop_price{0};
        Side _side;
        bool _cancelled{false};
        OrderParams _params;
        OrderType _type;
        std::time_t _expire_time;
};

class OrderComparator {
    
    public:
        OrderComparator(bool asc): _asc{asc} {}

        bool operator()(const std::shared_ptr<Order>& lhs, const std::shared_ptr<Order>& rhs) const {
            if (lhs->get_price() == rhs->get_price()) {
                return lhs->get_timestamp() < rhs->get_timestamp();
            }
            
            if(_asc){
                return lhs->get_price() < rhs->get_price();
            }else{
                return lhs->get_price() > rhs->get_price();
            }
        };

    private:
     bool _asc{false};
};

class DateOrderComparator {
    public:
        DateOrderComparator(bool asc): _asc{asc} {}

        bool operator()(const std::shared_ptr<Order>& lhs, const std::shared_ptr<Order>& rhs) const {
            if(_asc){
                return lhs->get_expire_time() < rhs->get_expire_time();
            }

            return lhs->get_expire_time() > rhs->get_expire_time();
        };

    private:
     bool _asc{false};
};

#endif
