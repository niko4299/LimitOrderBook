#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../data_structures/limit.hpp"
#include "../../data_structures/order.hpp"

class LimitFixture : public testing::Test {
 protected:
 
  virtual void SetUp() {
    limit = std::make_shared<Limit>(1000.0);
    order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY,OrderParams::STOP, OrderType::LIMIT);
  }

  std::shared_ptr<Limit> limit;
  std::shared_ptr<Order> order;
};


TEST_F(LimitFixture, TestEmpty){
  ASSERT_TRUE(limit->empty());
  limit->_head = order;
  ASSERT_FALSE(limit->empty());
}

TEST_F(LimitFixture, TestSide){
  ASSERT_EQ(limit->side(), Side::UNKOWN);
  limit->_head = order;
  ASSERT_EQ(limit->side(), Side::BUY);
}


TEST_F(LimitFixture, TestNumberOfOrders){
  limit->increase_number_of_orders();
  ASSERT_EQ(limit->get_number_of_active_orders(), 1);
  ASSERT_EQ(limit->get_total_number_of_orders(), 1);

  limit->decrease_number_of_orders();
  ASSERT_EQ(limit->get_number_of_active_orders(), 0);
  ASSERT_EQ(limit->get_total_number_of_orders(), 1);
}

TEST_F(LimitFixture, TestVolume){
  limit->increase_volume(10.0);
  ASSERT_EQ(limit->get_active_volume(), 10.0);
  ASSERT_EQ(limit->get_total_volume(), 10.0);

  limit->decrease_volume(5.0);
  ASSERT_EQ(limit->get_active_volume(), 5.0);
  ASSERT_EQ(limit->get_total_volume(), 10.0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
