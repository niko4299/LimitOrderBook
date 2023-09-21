#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>
#include <vector>

#include "../../exchange/exchange.hpp"

class MockTradeRepository : public TradeRepository {
public:
    MOCK_CONST_METHOD1(enqueue, void(const Trade& trade));
};

class MockOrderRepository: public OrderRepository {
public:
    MOCK_METHOD(void, enqueue, (std::shared_ptr<Order>& Order), (const));
};

// Mock OrderRepository and TradeRepository classes if needed

// Define a test fixture
class ExchangeTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

};

// Test the Exchange class
// TEST_F(ExchangeTest, AddOrder) {
    // Exchange exchange({"AAPL", "GOOG"}, 100, std::make_shared<OrderRepository>(), std::make_shared<MockTradeRepository>());

    // // Create and add an order to the exchange
    // auto order = std::make_shared<Order>("AAPL", /* other order parameters */);
    // exchange.add_order(std::move(order));

    // You can add assertions to verify that the order was added correctly
    // For example, check if the order is in the order book
    // ASSERT_TRUE(order_book_contains_order("AAPL", order_id));
// }

// TEST_F(ExchangeTest, ModifyOrder) {
//     Exchange exchange({"AAPL", "GOOG"}, 100, order_repository, trade_repository);

//     // Create and add an order to the exchange
//     auto order = std::make_shared<Order>("AAPL", /* other order parameters */);
//     exchange.add_order(std::move(order));

//     // Modify the order
//     auto modified_order = std::make_shared<Order>("AAPL", /* modified order parameters */);
//     exchange.modify_order(std::move(modified_order));

//     // You can add assertions to verify that the order was modified correctly
//     // For example, check if the order in the order book has been updated
//     // ASSERT_TRUE(order_book_contains_updated_order("AAPL", modified_order_id));
// }

// TEST_F(ExchangeTest, CancelOrder) {
//     Exchange exchange({"AAPL", "GOOG"}, 100, order_repository, trade_repository);

//     // Create and add an order to the exchange
//     auto order = std::make_shared<Order>("AAPL", /* other order parameters */);
//     exchange.add_order(std::move(order));

//     // Cancel the order
//     std::string order_id = /* get the order id */;
//     exchange.cancel_order("AAPL", order_id);

//     // You can add assertions to verify that the order was canceled correctly
//     // For example, check if the order in the order book has been removed
//     // ASSERT_FALSE(order_book_contains_order("AAPL", order_id));
// }

// Add more test cases as needed

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
