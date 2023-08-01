#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <iostream>
#include "data_structures/order.hpp"

#include <boost/atomic.hpp>

int producer_count = 0;
boost::atomic_int consumer_count (0);

boost::lockfree::spsc_queue<std::shared_ptr<Order>, boost::lockfree::capacity<1024> > spsc_queue;

const int iterations = 10000000;

void producer(void)
{
    for (int i = 0; i != iterations; ++i) {
        auto order = std::make_shared<Order>("order_id","f_instrument","test_user",100.5+i,1100.02,1100.02,Side::BUY, OrderParams::STOP, OrderType::LIMIT);

        while (!spsc_queue.push(order))
            ;
    }
}

boost::atomic<bool> done (false);

void consumer(void)
{
    std::shared_ptr<Order> value;
    while (!done) {
        while (spsc_queue.pop(value)){
            std::cout<<value->get_id()<<"\n";
            ++consumer_count;
    }
    }

    while (spsc_queue.pop(value))
        ++consumer_count;
}

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "boost::lockfree::queue is ";
    if (!spsc_queue.is_lock_free())
        cout << "not ";
    cout << "lockfree" << endl;

    boost::thread producer_thread(producer);
    boost::thread consumer_thread(consumer);

    producer_thread.join();
    done = true;
    consumer_thread.join();

    cout << "produced " << producer_count << " objects." << endl;
    cout << "consumed " << consumer_count << " objects." << endl;
}