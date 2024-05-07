#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../src/repository/order_repository.hpp"

class OrderRepositoryFixture : public testing::Test {
 protected:
  virtual void SetUp() {
    _order_repository = std::make_unique<OrderRepository>("./db_path_test", 100);
  }

  virtual void TearDown() {
    std::filesystem::remove_all(std::filesystem::path("./db_path_test"));
  }

  std::unique_ptr<OrderRepository> _order_repository;
};

TEST_F(OrderRepositoryFixture, TestSave){
  auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT, time(0));

  auto saved = _order_repository->save(order);

  ASSERT_TRUE(saved);
}

TEST_F(OrderRepositoryFixture, TestGet){
  std::string order_id = "order_id";
  auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT, time(0));
  auto saved = _order_repository->save(order);
  ASSERT_TRUE(saved);

  // Existing order
  {
    auto saved_order = _order_repository->get(order_id);
    ASSERT_TRUE(saved_order.has_value());
    ASSERT_EQ(order_id, saved_order.value()->get_id());
  }

  // Non-existen order
  {
    std::string non_existen_id = "random_id";
    auto saved_order = _order_repository->get(non_existen_id);
    ASSERT_FALSE(saved_order.has_value());
  }
}

TEST_F(OrderRepositoryFixture, TestRemove){
  auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT, time(0));

  auto saved = _order_repository->save(order);

  ASSERT_TRUE(saved);

  auto removed = _order_repository->remove(order->get_id());

  ASSERT_TRUE(removed);
  auto ret_order = _order_repository->get("order_id");
  ASSERT_FALSE(ret_order.has_value());

  removed = _order_repository->remove("not_existing_id");

  ASSERT_TRUE(removed);
}

TEST_F(OrderRepositoryFixture, TestGetAll){
  auto order_1 = std::make_shared<Order>("order_id_1","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT, time(0));

  auto saved = _order_repository->save(order_1);

  auto order_2 = std::make_shared<Order>("order_id_2","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::GTC, OrderType::LIMIT, time(0));

  saved = _order_repository->save(order_2);

  ASSERT_TRUE(saved);

  auto orders = _order_repository->get_all();

  ASSERT_EQ(orders.size(),2);
  ASSERT_EQ(orders[0]->get_id(),"order_id_1");
  ASSERT_EQ(orders[1]->get_id(),"order_id_2");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
