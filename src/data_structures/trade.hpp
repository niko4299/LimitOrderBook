#pragma once

#include <string>
#include <ctime>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

struct Trade{
    std::string buyer_id;
    std::string seller_id;
    std::string buyer_order_id;
    std::string seller_order_id;
    std::string instrument;
    float volume;
    float price;
    std::time_t timestamp;
};
