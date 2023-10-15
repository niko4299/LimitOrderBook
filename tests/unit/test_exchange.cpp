#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>

#include "../../src/exchange/exchange.hpp"

class ExchangeTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            _order_repository = std::make_shared<OrderRepository>("./db_path_test", 100);
            _trade_repository = std::make_shared<TradeRepository>("0.0.0.0", 1, 100);
        }

        virtual void TearDown() {
            std::filesystem::remove_all(std::filesystem::path("./db_path_test"));
        }

    std::shared_ptr<OrderRepository> _order_repository;
    std::shared_ptr<TradeRepository> _trade_repository;
};

TEST_F(ExchangeTest, AddOrder) {
    auto instruments_info = std::vector<std::pair<std::string, float>>{{"AAPL", 1000.0}, {"GOOGL", 1000.0}};
    auto exchange = std::make_shared<Exchange>(instruments_info, 100, _order_repository, _trade_repository);

    auto order = std::make_shared<Order>("order_id","AAPL","test_user",100.5,1000.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
    auto status = exchange->add_order(std::move(order));

    ASSERT_EQ(status, OrderStatus::ACCEPTED);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    ASSERT_TRUE(exchange->get_order("order_id").has_value());
}

TEST_F(ExchangeTest, CancelOrder) {
    auto instruments_info = std::vector<std::pair<std::string, float>>{{"AAPL", 1000.0}, {"GOOGL", 1000.0}};
    auto exchange = std::make_shared<Exchange>(instruments_info, 100, _order_repository, _trade_repository);

    auto order = std::make_shared<Order>("order_id","AAPL","test_user",100.5,1000.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
    exchange->add_order(std::move(order));

    exchange->cancel_order(order->get_instrument(),order->get_id());
   
       std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto maybe_order = exchange->get_order("order_id");

    ASSERT_TRUE(maybe_order.has_value());
    ASSERT_TRUE(maybe_order.value()->is_cancelled());
}

TEST_F(ExchangeTest, ModifyOrder) {
    auto instruments_info = std::vector<std::pair<std::string, float>>{{"AAPL", 1000.0}, {"GOOGL", 1000.0}};
    auto exchange = std::make_shared<Exchange>(instruments_info, 100, _order_repository, _trade_repository);

    auto order = std::make_shared<Order>("order_id","AAPL","test_user",100.5,1000.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);
    exchange->add_order(std::move(order));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto new_qty = 103.5;
    auto modify_order = std::make_shared<Order>("order_id","AAPL","test_user", new_qty,1000.02,Side::SELL, OrderParams::GTC, OrderType::LIMIT);

    exchange->modify_order(std::move(modify_order));

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto maybe_order = exchange->get_order("order_id");

    ASSERT_TRUE(maybe_order.has_value());
    ASSERT_EQ(maybe_order.value()->get_qty(), new_qty);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
