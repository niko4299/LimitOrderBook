#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../orderbook/orderbook.hpp"


class OrderBookFixture : public testing::Test  {
    protected:
  
        virtual void SetUp() {
            _orderbook = std::make_unique<OrderBook>("eth",1000.0,"./test_order_db/","./test_trade_db/");
        }

        virtual void TearDown() {
            std::filesystem::remove_all(std::filesystem::path("./test_order_db/"));
            std::filesystem::remove_all(std::filesystem::path("./test_trade_db/"));
        }
    
    std::unique_ptr<OrderBook> _orderbook;
};

TEST_F(OrderBookFixture, TestAddStopOrder){

    // Add limit bid stop order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,950.02,950.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),0);
    }

    // Add limit ask stop order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::SELL,OrderParams::STOP, OrderType::LIMIT);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),1);
    }

    // Add limit bid stop order that get processed immediatly
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_bids().size(),1);
    }

    // Add limit ask stop order that get processed immediatly
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,900.02,900.02,Side::SELL,OrderParams::STOP, OrderType::LIMIT);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_asks().size(), 1);
    }

    // Add market bid stop order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,950.02,950.02,Side::BUY,OrderParams::STOP, OrderType::MARKET);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),1);
    }

    // Add market ask stop order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::SELL,OrderParams::STOP, OrderType::MARKET);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),2);
    }

    // Add market bid stop order that get processed immediatly
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::MARKET);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_bids().size(),1);
    }

    // Add market ask stop order that get processed immediatly
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,900.02,900.02,Side::SELL,OrderParams::STOP, OrderType::MARKET);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_asks().size(), 1);
    }
}

TEST_F(OrderBookFixture, TestAddOrder){

    // Add GTC buy limit order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(order);

        ASSERT_EQ(_orderbook->get_bids().size(),1);
        ASSERT_EQ(_orderbook->get_asks().size(),0);
    }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}