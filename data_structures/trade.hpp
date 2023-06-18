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

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &a, const unsigned version){
        a & buyer_id & seller_id & buyer_order_id & seller_order_id & instrument & volume & price & timestamp;
    }
};