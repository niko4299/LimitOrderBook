#include <iostream>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

#include "ringbuffer.hpp"

class ThreadPool {
public:
    ThreadPool(std::size_t num_instruments, std::size_t ring_buffer_size) : _stop(false) {
        for (std::size_t i = 0; i < num_instruments; ++i) {
            add_thread(ring_buffer_size);
        }
    }

    ~ThreadPool() {
        _stop.store(true);
        for (std::jthread &worker : _workers) {
            worker.join();
        }
    }

    void add_thread(std::size_t ring_buffer_size){
        auto rb = std::make_shared<RingBuffer<std::function<void()>>>(ring_buffer_size);
        _task_ring_buffer.emplace_back(rb);
        _workers.emplace_back([this,rb] {
            while (!_stop.load()) {
                std::function<void()> task;

                if (rb->pop(task)) {
                    task();
                }
            }
        });
    }

    template <typename F>
    void enqueue(std::size_t idx, F&& f) {
        _task_ring_buffer[idx]->push(std::forward<F>(f));
    }

private:
    std::vector<std::jthread> _workers;
    std::vector<std::shared_ptr<RingBuffer<std::function<void()>>>> _task_ring_buffer;
    std::atomic_bool _stop;
};
