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
            _task_ring_buffer.emplace_back(RingBuffer<std::function<void()>>{ring_buffer_size});
            _workers.emplace_back([this,i] {
                auto idx = i;
                while (!_stop.load()) {
                    std::function<void()> task;

                    if (!_task_ring_buffer[idx]->pop(task)) {
                        task();
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        _stop.store(true);
        for (std::jthread &worker : _workers) {
            worker.join();
        }
    }

    // Add a task to the thread pool
    template <typename F>
    void enqueue(std::size_t idx, F&& f) {
        _task_ring_buffer[idx]->push(std::forward<F>(f));
    }

private:
    std::vector<std::jthread> _workers;
    std::vector<std::unique_ptr<RingBuffer<std::function<void()>>>> _task_ring_buffer;
    std::atomic_bool _stop;
};
