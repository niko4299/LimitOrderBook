#ifndef LIMIT_HPP
#define LIMIT_HPP

#include <cstdint>
#include <string>
#include <ctime>
#include <stdexcept>
#include <memory>
#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "enums.hpp"

class Order;

class Limit final {
    public:
        Limit() = default;

        Limit(float price);

        ~Limit() = default;

        float get_price();

        void clear();

        std::vector<std::shared_ptr<Order>> all_active_orders();

        float get_active_volume();

        float get_total_volume();

        void increase_volume(float delta_qty);

        void decrease_volume(float delta_qty);

        void increase_number_of_orders();

        void decrease_number_of_orders();

        std::uint64_t get_number_of_active_orders();

        std::uint64_t get_total_number_of_orders();

        bool empty();

        Side side();

    bool operator==(const Limit& other) const {
        return _price == other._price;
    }

    public:
        std::shared_ptr<Order> _head{};
        std::shared_ptr<Order> _tail{};

    private:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive &a, const unsigned version);

        float _price;
        std::uint64_t _number_of_active_orders{0};
        std::uint64_t _total_number_of_orders{0};
        double _active_volume{0};
        double _total_volume{0};
};

struct LimitComparatorAsc
{
  bool operator()(const std::shared_ptr<Limit>& lhs, const std::shared_ptr<Limit>& rhs) const
  {
   return *lhs < *rhs
  }
};


struct LimitComparatorDesc
{
  bool operator()(const std::shared_ptr<Limit>& lhs, const std::shared_ptr<Limit>& rhs) const
  {
   return *rhs < *lhs
  }
};

#endif
