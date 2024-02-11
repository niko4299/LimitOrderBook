#include<thread>
#include<memory>

#include "ringbuffer.hpp"

class FunctionTask {
    public:

        FunctionTask(std::size_t ringbuffer_size) {
            _ringbuffer = std::make_unique<RingBuffer<std::function<void()>>>(ringbuffer_size);
            _thread = std::jthread(std::bind_front(&FunctionTask::process_messages, this));
        }

        void process_messages(std::stop_token s){
            std::function<void()> task;      
            while(!s.stop_requested()){
                if (_ringbuffer->pop(task)) {
                    task();
                }      
            } 
        }

        template <typename F>
        void enqueue(F&& f) {
            _ringbuffer->push(std::forward<F>(f));
        }

        ~FunctionTask() = default;

    private:
        std::unique_ptr<RingBuffer<std::function<void()>>> _ringbuffer;
        std::jthread _thread;
};
