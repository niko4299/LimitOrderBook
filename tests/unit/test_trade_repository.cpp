#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../repository/trade_repository.hpp"
#include "../../utils/ringbuffer.hpp"

class TradeRepositoryFixture : public testing::Test {
    protected:
        virtual void SetUp() {
            _trade_repository = std::make_unique<TradeRepository>(_host, 1, spsc_queue);
        }

        virtual void TearDown(){
             _trade_repository = std::make_unique<TradeRepository>(_host, 1, spsc_queue);

            _trade_repository->run_query("TRUNCATE orderbook.trades;");
        }

  std::string _host = "0.0.0.0";
  std::unique_ptr<TradeRepository> _trade_repository;
  std::shared_ptr<RingBuffer<Trade>> spsc_queue;
};

TEST_F(TradeRepositoryFixture, TestSave){
    auto trade = Trade{"buyer_id","seller_id","buyer_order_id","seller_order_id","eth",100.01,150.5,time(0)};
    auto saved = _trade_repository->save(trade);

    ASSERT_TRUE(saved);
}

TEST_F(TradeRepositoryFixture, TestGetAll){
    auto trade = Trade{"buyer_id","seller_id","buyer_order_id","seller_order_id","eth",100.01,150.5,time(0)};
    auto saved = _trade_repository->save(trade);

    ASSERT_TRUE(saved);

    auto all_trades = _trade_repository->get_all_trades();
    ASSERT_EQ(all_trades.size(),1);
}

TEST_F(TradeRepositoryFixture, TestGetByPrimaryKey){
    auto timestamp = time(0);
    auto buyer_id = "buyer_id_1";
    auto seller_id = "seller_id_1";
    auto trade = Trade{buyer_id, seller_id,"buyer_order_id","seller_order_id","eth",100.01,150.5,timestamp};
    auto saved = _trade_repository->save(trade);

    ASSERT_TRUE(saved);

    auto maybe_trade = _trade_repository->get_trade_by_primary_key(buyer_id, seller_id,timestamp);
    ASSERT_TRUE(maybe_trade.has_value());
    ASSERT_EQ(maybe_trade.value().buyer_id,buyer_id);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
