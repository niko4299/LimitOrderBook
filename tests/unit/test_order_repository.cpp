#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../../repository/order_repository.hpp"

class OrderRepositoryFixture : public testing::Test {
 protected:
  virtual void SetUp() {
    _order_repository = std::make_unique<OrderRepository>(db_file_path);
  }

  virtual void TearDown() {
    std::filesystem::remove_all(std::filesystem::path(db_file_path));
  }

  std::string db_file_path = "./db_path_test";
  std::unique_ptr<OrderRepository> _order_repository;
};

TEST_F(OrderRepositoryFixture, TestSave){
  auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT);

  auto saved = _order_repository->save(order);

  ASSERT_TRUE(saved);
}

TEST_F(OrderRepositoryFixture, TestGet){
  std::string order_id = "order_id";
  auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT);
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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



