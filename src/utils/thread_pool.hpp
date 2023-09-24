#include <iostream>
#include <vector>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

#include "function_task.hpp"

class ThreadPool {
    public:
        ThreadPool(std::size_t num_instruments, std::size_t ring_buffer_size) {
            for (std::size_t i = 0; i < num_instruments; ++i) {
                _workers.emplace_back(std::make_unique<FunctionTask>(ring_buffer_size));
            }
        }

        ~ThreadPool() = default;

        template <typename F>
        void enqueue(std::size_t idx, F&& f) {
            _workers[idx]->enqueue(std::forward<F>(f));
        }

    private:
        std::vector<std::unique_ptr<FunctionTask>> _workers;
};
