#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../../storage/rbtree.hpp"

class RBTreeFixture : public testing::Test {
 protected:
  virtual void SetUp() {
    rbtree = RBTree<int>{};
  }

  RBTree<int> rbtree;
};

TEST_F(RBTreeFixture, TestContainsNonExistingNumber){
  auto non_existing_number = 10;
  auto test_numbers = {12,9,8,6,5,7,6,6};
  
  for(auto& number: test_numbers){
        rbtree.insert(number);
    }

  auto containes = rbtree.contains(non_existing_number);
  ASSERT_EQ(containes,false);
}

TEST_F(RBTreeFixture, TestFindNonExistingNumber){
  auto non_existing_number = 10;
  auto test_numbers = {12,9,8,6,5,7,6,6};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }

  auto maybe_value = rbtree.find(non_existing_number);
  ASSERT_EQ(maybe_value.has_value(),false);
}

TEST_F(RBTreeFixture, TestContainsExistingNumber){
  auto test_numbers = {12,9,8,6,5,7,6,6};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }
  
  auto containes = rbtree.contains(9);
  ASSERT_EQ(containes,true);
}

TEST_F(RBTreeFixture, TestFindExistingNumber){
  auto test_numbers = {12,9,8,6,5,7,6,6};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }

  auto maybe_value = rbtree.find(6);
  ASSERT_EQ(maybe_value.has_value(),true);
  ASSERT_EQ(maybe_value,6);
}

TEST_F(RBTreeFixture, TestInsertRoot){
  auto root_number = 5;
  rbtree.insert(root_number);
  auto x = rbtree.find(root_number);
  auto root = rbtree.get_root();
  ASSERT_EQ(x,root_number);
  ASSERT_EQ(root->key,root_number);
  ASSERT_EQ(root->is_black,true);
  ASSERT_EQ(rbtree.size(),1);
}

TEST_F(RBTreeFixture, TestInsertRightRotation){
  auto test_numbers = {5,10,12};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }
  auto root = rbtree.get_root();
  ASSERT_EQ(root->key,10);
  ASSERT_EQ(root->is_black,true);
  ASSERT_EQ(root->left->key,5);
  ASSERT_EQ(root->left->is_black,false);
  ASSERT_EQ(root->right->key,12);
  ASSERT_EQ(root->right->is_black,false);
  ASSERT_EQ(rbtree.size(),test_numbers.size());
}


TEST_F(RBTreeFixture, TestInsertLeftRotation){
  auto test_numbers = {5,4,3};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }
  auto root = rbtree.get_root();
  ASSERT_EQ(root->key,4);
  ASSERT_EQ(root->is_black,true);
  ASSERT_EQ(root->left->key,3);
  ASSERT_EQ(root->left->is_black,false);
  ASSERT_EQ(root->right->key,5);
  ASSERT_EQ(root->right->is_black,false);
  ASSERT_EQ(rbtree.size(),test_numbers.size());
}

TEST_F(RBTreeFixture, TestReColoringAfterAddingLeafNode){
  auto test_numbers = {6,5,3,4};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }
  auto root = rbtree.get_root();
  ASSERT_EQ(root->key,5);
  ASSERT_EQ(root->is_black,true);
  ASSERT_EQ(root->left->key,3);
  ASSERT_EQ(root->left->is_black,true);
  ASSERT_EQ(root->right->key,6);
  ASSERT_EQ(root->right->is_black,true);
  ASSERT_EQ(rbtree.size(),test_numbers.size());
}

TEST_F(RBTreeFixture, TestInsertBigRightRootRotation){
  auto test_numbers = {12,9,8,6,5,7,6,6};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }

  auto root = rbtree.get_root();
  ASSERT_EQ(root->key,7);
  ASSERT_EQ(root->is_black,true);
  ASSERT_EQ(root->left->key,6);
  ASSERT_EQ(root->left->is_black,false);
  ASSERT_EQ(root->right->key,9);
  ASSERT_EQ(root->right->is_black,false);
  ASSERT_EQ(root->left->left->key,5);
  ASSERT_EQ(root->left->left->is_black,true);
  ASSERT_EQ(root->left->right->key,6);
  ASSERT_EQ(root->left->right->is_black,true);
  ASSERT_EQ(root->left->right->right->key,6);
  ASSERT_EQ(root->left->right->right->is_black,false);
  ASSERT_EQ(root->right->left->key,8);
  ASSERT_EQ(root->right->left->is_black,true);
  ASSERT_EQ(root->right->right->key,12);
  ASSERT_EQ(root->right->right->is_black,true);
  ASSERT_EQ(rbtree.size(),test_numbers.size());
}

TEST_F(RBTreeFixture, TestInsertBigLeftRootRotation){
  auto test_numbers = {5,30,45,60,2,63,46,47,44};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }

  auto root = rbtree.get_root();
  ASSERT_EQ(root->key,46);
  ASSERT_EQ(root->is_black,true);
  ASSERT_EQ(root->left->key,30);
  ASSERT_EQ(root->left->is_black,false);
  ASSERT_EQ(root->right->key,60);
  ASSERT_EQ(root->right->is_black,false);
  ASSERT_EQ(root->left->left->key,5);
  ASSERT_EQ(root->left->left->is_black,true);
  ASSERT_EQ(root->left->right->key,45);
  ASSERT_EQ(root->left->right->is_black,true);
  ASSERT_EQ(root->left->right->left->key,44);
  ASSERT_EQ(root->left->right->left->is_black,false);
  ASSERT_EQ(root->left->left->left->key,2);
  ASSERT_EQ(root->left->left->left->is_black,false);
  ASSERT_EQ(root->right->left->key,47);
  ASSERT_EQ(root->right->left->is_black,true);
  ASSERT_EQ(root->right->right->key,63);
  ASSERT_EQ(root->right->right->is_black,true);
  ASSERT_EQ(rbtree.size(),test_numbers.size());
}

TEST_F(RBTreeFixture, TestClear){
    auto test_numbers = {5,30,45,60,2,63,46,47,44};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }

  rbtree.clear();
  auto contains = rbtree.contains(5);
  
  ASSERT_EQ(contains, false);
  ASSERT_EQ(rbtree.size(), 0);
}

TEST_F(RBTreeFixture, TestDelete){
  auto test_numbers = {5,30,45,60,2,63,46,47,44};

  for(auto& number: test_numbers){
    rbtree.insert(number);
  }

  // Remove leaf
  {
    rbtree.remove(2);
    auto root = rbtree.get_root();
    ASSERT_EQ(root->key,46);
    ASSERT_EQ(root->is_black,true);
    ASSERT_EQ(root->left->key,30);
    ASSERT_EQ(root->left->is_black,false);
    ASSERT_EQ(root->right->key,60);
    ASSERT_EQ(root->right->is_black,false);
    ASSERT_EQ(root->left->left->key,5);
    ASSERT_EQ(root->left->left->is_black,true);
    ASSERT_EQ(root->left->right->key,45);
    ASSERT_EQ(root->left->right->is_black,true);
    ASSERT_EQ(root->left->right->left->key,44);
    ASSERT_EQ(root->left->right->left->is_black,false);
    ASSERT_EQ(root->right->left->key,47);
    ASSERT_EQ(root->right->left->is_black,true);
    ASSERT_EQ(root->right->right->key,63);
    ASSERT_EQ(root->right->right->is_black,true);
    ASSERT_EQ(rbtree.size(),test_numbers.size() - 1);
  }

  // Remove root left node, left rotation
  {
    rbtree.remove(30);
    auto root = rbtree.get_root();
    ASSERT_EQ(root->key,46);
    ASSERT_EQ(root->is_black,true);
    ASSERT_EQ(root->left->key,44);
    ASSERT_EQ(root->left->is_black,false);
    ASSERT_EQ(root->right->key,60);
    ASSERT_EQ(root->right->is_black,false);
    ASSERT_EQ(root->left->left->key,5);
    ASSERT_EQ(root->left->left->is_black,true);
    ASSERT_EQ(root->left->right->key,45);
    ASSERT_EQ(root->left->right->is_black,true);
    ASSERT_EQ(root->right->left->key,47);
    ASSERT_EQ(root->right->left->is_black,true);
    ASSERT_EQ(root->right->right->key,63);
    ASSERT_EQ(root->right->right->is_black,true);
    ASSERT_EQ(rbtree.size(),test_numbers.size() - 2);
  }

  // Remove root right node, no rotation
  {
    rbtree.remove(60);
    auto root = rbtree.get_root();
    ASSERT_EQ(root->key,46);
    ASSERT_EQ(root->is_black,true);
    ASSERT_EQ(root->left->key,44);
    ASSERT_EQ(root->left->is_black,false);
    ASSERT_EQ(root->right->key,63);
    ASSERT_EQ(root->right->is_black,true);
    ASSERT_EQ(root->left->left->key,5);
    ASSERT_EQ(root->left->left->is_black,true);
    ASSERT_EQ(root->left->right->key,45);
    ASSERT_EQ(root->left->right->is_black,true);
    ASSERT_EQ(root->right->left->key,47);
    ASSERT_EQ(root->right->left->is_black,false);
    ASSERT_EQ(rbtree.size(),test_numbers.size() - 3);
  }

  // Remove root node, rebalance tree 
  {
    rbtree.remove(46);
    auto root = rbtree.get_root();
    ASSERT_EQ(root->key,47);
    ASSERT_EQ(root->is_black,true);
    ASSERT_EQ(root->left->key,44);
    ASSERT_EQ(root->left->is_black,false);
    ASSERT_EQ(root->right->key,63);
    ASSERT_EQ(root->right->is_black,true);
    ASSERT_EQ(root->left->left->key,5);
    ASSERT_EQ(root->left->left->is_black,true);
    ASSERT_EQ(root->left->right->key,45);
    ASSERT_EQ(root->right->is_black,true);
    ASSERT_EQ(rbtree.size(),test_numbers.size() - 4);
  }

  // Delete all
  {
    for(auto& number : {47,44,63,45,5}){
      rbtree.remove(number);
    }
    
    ASSERT_EQ(rbtree.size(), 0);
  }
}

TEST_F(RBTreeFixture, TestFirst){
  ASSERT_EQ(rbtree.first().has_value(),false);
  
  rbtree.insert(8);
  ASSERT_EQ(rbtree.first(),8);

  rbtree.insert(10);
  ASSERT_EQ(rbtree.first(),8);
  
  rbtree.insert(2);
  ASSERT_EQ(rbtree.first(),2);

  rbtree.remove(2);
  ASSERT_EQ(rbtree.first(),8);

  // Test removing most left that has right child
  {
    rbtree.clear();
    auto numbers = {1000,2000,800,600,750,630};
    for(auto& number: numbers){
      rbtree.insert(number);
    }
    ASSERT_EQ(rbtree.first(),600);
    rbtree.remove(600);
    ASSERT_EQ(rbtree.first(),630);
  }
}


TEST_F(RBTreeFixture, TestLast){
  ASSERT_EQ(rbtree.last().has_value(),false);
  
  rbtree.insert(8);
  ASSERT_EQ(rbtree.last(),8);

  rbtree.insert(10);
  ASSERT_EQ(rbtree.last(),10);
  
  rbtree.insert(11);
  ASSERT_EQ(rbtree.last(),11);

  rbtree.insert(2);
  ASSERT_EQ(rbtree.last(),11);

  rbtree.remove(11);
  ASSERT_EQ(rbtree.last(),10);

  // Test removing most right that has left child
  {
    rbtree.clear();
    auto numbers = {2000,500,3000,2800};
    for(auto& number: numbers){
      rbtree.insert(number);
    }
    ASSERT_EQ(rbtree.last(),3000);
    rbtree.remove(3000);
    ASSERT_EQ(rbtree.last(),2800);
  }
}

TEST_F(RBTreeFixture, TestForwardIterator){
    auto numbers = {2000,500,3000,2800};
    for(auto& number: numbers){
      rbtree.insert(number);
    }
    
    std::vector<int> expected_values{500,2000,2800,3000};
    int i = 0;
    for(auto& value: rbtree){
      ASSERT_EQ(value,expected_values[i]);
      i++;
    }

    i = 0;
    for(auto it = rbtree.begin(); it != rbtree.end();it++){
      ASSERT_EQ(*it,expected_values[i]);
      i++;
    }

    ASSERT_EQ(i,expected_values.size() - 1);
    i = expected_values.size()-1;
    for(auto it = rbtree.end(); it != rbtree.begin();it--){
      ASSERT_EQ(*it,expected_values[i]);
      i--;
    } 
}

TEST_F(RBTreeFixture, TestReverseIterator){
    auto numbers = {2000,500,3000,2800};
    for(auto& number: numbers){
      rbtree.insert(number);
    }
    
    std::vector<int> expected_values{3000,2800,2000,500};

    int i = 0;
    for(auto it = rbtree.rbegin(); it != rbtree.rend();it++){
      ASSERT_EQ(*it,expected_values[i]);
      i++;
    }

    ASSERT_EQ(i,expected_values.size() - 1);
    i = expected_values.size()-1;
    for(auto it = rbtree.rend(); it != rbtree.rbegin();it--){
      ASSERT_EQ(*it,expected_values[i]);
      i--;
    } 
}


TEST_F(RBTreeFixture, TestLowerBound){
    auto numbers = {2000,500,3000,2800};
    for(auto& number: numbers){
      rbtree.insert(number);
    }
    
    auto number = 1000;
    ASSERT_EQ(*rbtree.lower_bound(number),2000);
    number = 500;
    ASSERT_EQ(*rbtree.lower_bound(number),500);
    number = 2600;
    ASSERT_EQ(*rbtree.lower_bound(number),2800);
    number = 5000;
    ASSERT_EQ(*rbtree.lower_bound(number),3000);
    number = 100;
    ASSERT_EQ(*rbtree.lower_bound(number),500);
}


TEST_F(RBTreeFixture, TestUperBound){
    auto numbers = {2000,500,3000,2800};
    for(auto& number: numbers){
      rbtree.insert(number);
    }
    auto number = 1000;
    ASSERT_EQ(*rbtree.upper_bound(number),2000);
    number = 500;
    ASSERT_EQ(*rbtree.upper_bound(number),2000);
    number = 2600;
    ASSERT_EQ(*rbtree.upper_bound(number),2800);
    number = 5000;
    ASSERT_EQ(*rbtree.upper_bound(number),3000);
    number = 100;
    ASSERT_EQ(*rbtree.upper_bound(number),500);
}

TEST_F(RBTreeFixture, TestRange){
    auto numbers = {2000,500,3000,2800,1500,1780,4000,6000};
    for(auto& number: numbers){
      rbtree.insert(number);
    }

    // Between two existing values
    {
        auto range_iterators = rbtree.range(1500,4000);
        int i = 0;
        std::vector<int> expected_numbers{1500,1780,2000,2800,3000,4000};
        for(auto it = range_iterators.first; it != range_iterators.second; it++){
        
          ASSERT_EQ(*it,expected_numbers[i]);
          i++;
        }
        ASSERT_EQ(i,6);
    }

    // Between existing value and out of bound
    {
        auto range_iterators = rbtree.range(2800,7000);
        int i = 0;
        std::vector<int> expected_numbers{2800,3000,4000};
        for(auto it = range_iterators.first; it != range_iterators.second; it++){
        
          ASSERT_EQ(*it,expected_numbers[i]);
          i++;
        }
        ASSERT_EQ(i,3);
    }

    // Between out of bound value and existing
    {
        auto range_iterators = rbtree.range(-1000,1500);
        int i = 0;
        std::vector<int> expected_numbers{500,1500};
        for(auto it = range_iterators.first; it != range_iterators.second; it++){
        
          ASSERT_EQ(*it,expected_numbers[i]);
          i++;
        }
        ASSERT_EQ(i,2);
    }

    // Between two out of bound values
    {
        auto range_iterators = rbtree.range(-1000,7000);
        int i = 0;
        std::vector<int> expected_numbers{500,1500,1780,2000,2800,3000,4000};
        for(auto it = range_iterators.first; it != range_iterators.second; it++){
        
          ASSERT_EQ(*it,expected_numbers[i]);
          i++;
        }
        ASSERT_EQ(i,7);
    }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}