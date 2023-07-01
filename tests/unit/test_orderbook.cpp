#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../orderbook/orderbook.hpp"


class OrderBookFixture : public testing::Test  {
    protected:
  
        virtual void SetUp() {
            _orderbook = std::make_unique<OrderBook>("eth","./test_order_db/","./test_trade_db/");
        }

        virtual void TearDown() {
            std::filesystem::remove_all(std::filesystem::path(db_file_path));
        }
    
    std::unique_ptr<OrderBook> _orderbook;
};

TEST_F(OrderBookFixture, TestAddOrder){
    auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT);
    _orderbook->add_order(order);
    auto bid_stop_order = _orderbook->get_bid_stop_orders();
    ASSERT_EQ(bid_stop_order.size(),1);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}