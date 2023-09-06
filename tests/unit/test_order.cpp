#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "../../data_structures/order.hpp"
#include "../../data_structures/limit.hpp"


class OrderFixture : public testing::Test {
 protected:
  virtual void SetUp() {
    order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5,1100.02,1100.02,Side::BUY, OrderParams::STOP, OrderType::LIMIT);
  }

  std::shared_ptr<Order> order;
};

TEST_F(OrderFixture, TestIsBuy){
  ASSERT_TRUE(order->is_buy());
}

TEST_F(OrderFixture, TestCancelAndIsCancelled){
  ASSERT_FALSE(order->is_cancelled());
  
  order->cancel();
  ASSERT_TRUE(order->is_cancelled());
}

TEST_F(OrderFixture, TestFillAndIsfullfilled){
  ASSERT_FALSE(order->is_fullfilled());
  
  order->fill();
  ASSERT_TRUE(order->is_fullfilled());
}

TEST_F(OrderFixture, TestSetGetLimitParent){
  auto test_limit = std::make_shared<Limit>();

  order->set_limit_parent(test_limit);
  ASSERT_EQ(order->get_limit_parent(), test_limit);
}


TEST_F(OrderFixture, TestModifyQty){
  
  // Test increase
  {
    order->increase_qty(10);
    ASSERT_EQ(order->get_qty(),110.5);
  }

  // Test decrease
  {
    order->decrease_qty(10);
    ASSERT_EQ(order->get_qty(),100.5);
  }

  // Test decrease more than qty
  {
    ASSERT_THROW(order->decrease_qty(1000),std::invalid_argument);
  }
}

TEST_F(OrderFixture, TestHasParam){
  ASSERT_TRUE(order->has_param(OrderParams::STOP));

  {
    order->set_params(OrderParams::FOK);

    ASSERT_FALSE(order->has_param(OrderParams::STOP));
    ASSERT_TRUE(order->has_param(OrderParams::AON));
    ASSERT_TRUE(order->has_param(OrderParams::IOC));
    ASSERT_TRUE(order->has_param(OrderParams::FOK));
    ASSERT_FALSE(order->has_param(OrderParams::GFD));
  }

  {
    order->set_params(OrderParams::IOC);
    ASSERT_FALSE(order->has_param(OrderParams::AON));
    ASSERT_TRUE(order->has_param(OrderParams::IOC));
    ASSERT_TRUE(order->has_param(OrderParams::FOK));
  }


  {
    order->set_params(OrderParams::AON);
    ASSERT_TRUE(order->has_param(OrderParams::AON));
    ASSERT_FALSE(order->has_param(OrderParams::IOC));
    ASSERT_TRUE(order->has_param(OrderParams::FOK));
  }
}

TEST_F(OrderFixture, TestGetType){
  ASSERT_EQ(order->get_type(), OrderType::LIMIT);
}

TEST_F(OrderFixture, TestSerializationDesrialization){
    std::ostringstream oss;
    boost::archive::text_oarchive oa(oss);
    oa << order;

    std::shared_ptr<Order> order_from_db;
    std::istringstream iss(oss.str());
    boost::archive::text_iarchive ia(iss);
    ia >> order_from_db;

    std::ostringstream oss2;

    boost::archive::text_oarchive oa2(oss2);
    oa2 << order_from_db;

    ASSERT_EQ(oss.str(), oss2.str());
    ASSERT_EQ(order_from_db->get_id(), order->get_id());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
