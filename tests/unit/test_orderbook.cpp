#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/orderbook/orderbook.hpp"

class OrderBookFixture : public testing::Test  {
    protected:
  
        virtual void SetUp() {
            _orderbook = std::make_unique<OrderBook>("eth",1000.0,_order_repository,_trade_repository);
        }

        virtual void TearDown() {
            _orderbook = std::make_unique<OrderBook>("eth",1000.0,_order_repository,_trade_repository);
        }
        
    std::shared_ptr<OrderRepository> _order_repository = std::make_shared<OrderRepository>("./db_path",100);
    std::shared_ptr<TradeRepository> _trade_repository = std::make_shared<TradeRepository>("0.0.0.0",100,100);
    std::unique_ptr<OrderBook> _orderbook;
};

TEST_F(OrderBookFixture, TestAddStopOrder){

    // Add limit bid stop order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,950.02,950.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),0);
    }

    // Add limit ask stop order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::SELL,OrderParams::STOP, OrderType::LIMIT);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),1);
    }

    // Add limit bid stop order that get processed immediatly
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_bids().size(),1);
    }

    // Add limit ask stop order that get processed immediatly
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,900.02,900.02,Side::SELL,OrderParams::STOP, OrderType::LIMIT);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),1);
        ASSERT_EQ(_orderbook->get_asks().size(), 0);
        ASSERT_EQ(_orderbook->get_bids().size(), 0);
    }

    // Add market bid stop order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,950.02,950.02,Side::BUY,OrderParams::STOP, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),1);
    }

    // Add market ask stop order
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1200.02,1200.02,Side::SELL,OrderParams::STOP, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),2);
    }

    // Add market bid stop order that get processed immediatly
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_bids().size(),0);
    }

    // Add market ask stop order that get processed immediatly
    {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,900.02,900.02,Side::SELL,OrderParams::STOP, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bid_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_ask_stop_orders().size(),2);
        ASSERT_EQ(_orderbook->get_asks().size(), 0);
    }
}

TEST_F(OrderBookFixture, TestAddGTCOrder){
    auto buy_order_id = "order_id_1";
    auto sell_order_id = "order_id_2";

    // Add GTC buy limit order
    {
        auto order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bids().size(),1);
        ASSERT_EQ(_orderbook->get_asks().size(),0);
    }

    // Add GTC sell limit order
    {
        auto order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,1000.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bids().size(), 1);
        ASSERT_EQ(_orderbook->get_asks().size(), 1);
    }

    // Cancel GTC buy limit order
    {
        _orderbook->cancel_order(buy_order_id);
        ASSERT_EQ(_orderbook->get_bids().size(), 0);
        ASSERT_EQ(_orderbook->get_asks().size(), 1);
    }

    // Cancel GTC sell limit order
    {
        _orderbook->cancel_order(sell_order_id);
        ASSERT_EQ(_orderbook->get_bids().size(), 0);
        ASSERT_EQ(_orderbook->get_asks().size(), 0);
    }
}

TEST_F(OrderBookFixture, TestAddIOCOrder){
    auto buy_order_id = "order_id_1";
    auto sell_order_id = "order_id_2";

    // Add IOC buy market order which gets cancelled
    {
        auto order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::IOC, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),0);
    }

    // Add IOC buy market order which gets matched
    {
        auto limit_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,950.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));

        auto market_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::IOC, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),0);
    }

    // Add IOC sell market order which gets cancelled
    {
        auto order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,950.02,Side::SELL, OrderParams::IOC, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),0);
    }

    // Add IOC sell market order which gets matched
    {
        auto limit_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));

        auto market_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,950.02,Side::SELL, OrderParams::IOC, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),0);
    }
}

TEST_F(OrderBookFixture, TestAddAONOrder){
    auto buy_order_id = "order_id_1";
    auto sell_order_id = "order_id_2";

    // Add AON buy market order which gets cancelled
    {
        auto order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::AON, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),0);
    }

    // Add AON buy market order which gets cancelled because there is not same or bigger size sell limit
    {
        auto limit_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",80.5,950.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));

        auto market_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",99.5,950.02,Side::BUY, OrderParams::AON, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }

    // Add AON buy market order which get matched
    {
        auto limit_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,950.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));
        ASSERT_EQ(_orderbook->get_asks().size(), 2);

        auto market_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::AON, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }

    // Add AON sell market order which gets cancelled
    {
        auto order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,950.02,Side::SELL, OrderParams::AON, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }

    // Add AON sell market order which gets cancelled because there is not same or bigger size buy limit
    {
        auto limit_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",80.5,600.02, Side::BUY, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));
        ASSERT_EQ(_orderbook->get_bids().size(),1);


        auto market_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",99.5,600.02,Side::SELL, OrderParams::AON, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),1);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }

    // Add AON sell market order which gets matched
    {
        auto limit_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,600.02, Side::BUY, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));
        ASSERT_EQ(_orderbook->get_bids().size(),2);

        auto market_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,600.02,Side::SELL, OrderParams::AON, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),1);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }
}

TEST_F(OrderBookFixture, TestAddAFOKOrder){
    auto buy_order_id = "order_id_1";
    auto sell_order_id = "order_id_2";

    // Add AON buy market order which gets cancelled
    {
        auto order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::FOK, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),0);
    }

    // Add AON buy market order which gets cancelled because there is not same or bigger size sell limit
    {
        auto limit_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",80.5,950.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));

        auto market_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",99.5,950.02,Side::BUY, OrderParams::FOK, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }

    // Add AON buy market order which get matched
    {
        auto limit_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,950.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));
        ASSERT_EQ(_orderbook->get_asks().size(), 2);

        auto market_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,950.02,Side::BUY, OrderParams::FOK, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }

    // Add AON sell market order which gets cancelled
    {
        auto order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,950.02,Side::SELL, OrderParams::FOK, OrderType::MARKET);
        _orderbook->add_order(std::move(order));

        ASSERT_EQ(_orderbook->get_bids().size(),0);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }

    // Add AON sell market order which gets cancelled because there is not same or bigger size buy limit
    {
        auto limit_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",80.5,600.02, Side::BUY, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));
        ASSERT_EQ(_orderbook->get_bids().size(),1);


        auto market_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",99.5,600.02,Side::SELL, OrderParams::FOK, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),1);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }

    // Add AON sell market order which gets matched
    {
        auto limit_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",100.5,600.02, Side::BUY, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(limit_order));
        ASSERT_EQ(_orderbook->get_bids().size(),2);

        auto market_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",100.5,600.02,Side::SELL, OrderParams::FOK, OrderType::MARKET);
        _orderbook->add_order(std::move(market_order));

        ASSERT_EQ(_orderbook->get_bids().size(),1);
        ASSERT_EQ(_orderbook->get_asks().size(),1);
    }
}

TEST_F(OrderBookFixture, TestGetprice){
    auto buy_order_id = "order_id_1";
    auto sell_order_id = "order_id_2";

    // Empty price
    {
       auto price = _orderbook->get_spread();
       ASSERT_FLOAT_EQ(price.best_ask_price, -1);
       ASSERT_FLOAT_EQ(price.best_bid_price, -1);
       ASSERT_FLOAT_EQ(price.market_price, 0);
    }

    // Non empty-price
    {
        auto buy_limit_order = std::make_shared<Order>(buy_order_id,"f_instrument","test_user",80.5,600.02, Side::BUY, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(buy_limit_order));

        auto sell_limit_order = std::make_shared<Order>(sell_order_id,"f_instrument","test_user",80.5,950.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
        _orderbook->add_order(std::move(sell_limit_order));

        auto price = _orderbook->get_spread();
        ASSERT_FLOAT_EQ(price.best_ask_price, 950.02);
        ASSERT_FLOAT_EQ(price.best_bid_price, 600.02);
        ASSERT_FLOAT_EQ(price.market_price, 350.00);
    }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
