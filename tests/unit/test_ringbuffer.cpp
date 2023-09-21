#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "../../utils/ringbuffer.hpp"

// Define a test fixture
class RingBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        _ringbuffer = std::make_unique<RingBuffer<int>>(_capacity);
    }

    std::size_t _capacity = 10;
    std::unique_ptr<RingBuffer<int>> _ringbuffer;
};

TEST_F(RingBufferTest, InsertPopMultiSaftey) {
    std::vector<std::jthread> threads;
    std::vector<int> test_vector = {4,5,7,2,6,7,8,9};
    int sum = 0;

    for (auto i : test_vector){
        threads.push_back(std::jthread([&](int n){ _ringbuffer->push(n); }, i));
        sum += i;
    }

    int test_sum = 0;

    for(auto i = 0; i < test_vector.size(); i++){
        threads.push_back(std::jthread([&](){  
            int n;
            while(true){
                if(_ringbuffer->pop(n)){
                    break;
                }
            }
            test_sum += n;
            })
        );
    }

    for (auto& elem : threads){
        elem.join();
    }

    EXPECT_EQ(sum, test_sum);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
