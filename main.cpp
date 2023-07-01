#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
#include <numeric>
#include <vector>
#include <random>
#include <set>
#include <chrono>
#include <algorithm>
#include "storage/rbtree.hpp"
#include "data_structures/limit.hpp"

std::random_device rd;
std::mt19937 g(rd());

class Fake {
    public:
        Fake(float price):price{price}{}

        float price{0};

    private:
        bool operator>(const Fake& other){
            return price > other.price;
        }

};

int main(){

    auto x = std::make_shared<Fake>(100.1); 
    auto y = std::make_shared<Fake>(100.2);

    // auto s = std::make_shared<Limit>(100.1);
    auto z = x < y; 
    if(z){
    std::cout<< "TU";
    }
    // constexpr size_t SIZE = 100000;
    // std::vector<int> v (SIZE);
    // std::iota(v.begin(), v.end(), 1);
    // std::shuffle(v.begin(), v.end(), g);
    // RBTree<int> rbtree;
    // auto t1 = std::chrono::steady_clock::now();
    // for (auto n : v) {
    //     rbtree.insert(n);
    // }
    // auto t2 = std::chrono::steady_clock::now();
    // auto dt1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    // std::set<int> rbset;
    // t1 = std::chrono::steady_clock::now();
    // for (auto n : v) {
    //     rbset.insert(n);
    // }
    // t2 = std::chrono::steady_clock::now();
    // auto dt2 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    // std::cout << "Inserting " << SIZE << " elements:\n";
    // std::cout << "my red-black tree : " << dt1.count() << " ms\n";
    // std::cout << "standard red-black tree : " << dt2.count() << " ms\n";

    // t1 = std::chrono::steady_clock::now();
    // for (auto n : v) {
    //     rbset.find(n);
    // }
    // t2 = std::chrono::steady_clock::now();
    // dt2 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    // t1 = std::chrono::steady_clock::now();
    // for (auto n : v) {
    //     rbset.find(n);
    // }
    // t2 = std::chrono::steady_clock::now();
    // dt2 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "Finding " << SIZE << " elements:\n";
    // std::cout << "my ptr red-black tree : " << dt1.count() << " ms\n";
    // std::cout << "standard red-black tree : " << dt2.count() << " ms\n";

    // std::shuffle(v.begin(), v.end(), g);

    // t1 = std::chrono::steady_clock::now();
    // for (auto n : v) {
    //     rbtree.remove(n);
    // }
    // t2 = std::chrono::steady_clock::now();
    // auto dt3 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    // t1 = std::chrono::steady_clock::now();
    // for (auto n : v) {
    //     rbset.erase(n);
    // }
    // t2 = std::chrono::steady_clock::now();
    // auto dt4 = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    // std::cout << "Deleting " << SIZE << " elements:\n";
    // std::cout << "my red-black tree : " << dt3.count() << " ms\n";
    // std::cout << "standard red-black tree : " << dt4.count() << " ms\n";


    return 0;
}