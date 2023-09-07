#include<atomic>
#include<memory>
#include<vector>

template<typename T>
class RingBuffer {

    public:

        RingBuffer(std::size_t capacity): _capacity{capacity}, _data(capacity) {}

        ~RingBuffer() = default;

        template <typename Arg>
        void push(Arg&& value){
            std::uint64_t write_pos;
            std::uint64_t next_write_pos;
            while(true){
                write_pos = _write_pos.load(std::memory_order_relaxed);
                next_write_pos = write_pos + 1;
                if (next_write_pos == _read_pos_cache) {
                    _read_pos_cache = _read_pos.load(std::memory_order_acquire);
                    if (next_write_pos != _read_pos_cache) {
                        break;
                    }
                } else{
                    break;
                }
            }

            _data[write_pos] = T(std::forward<Arg>(value));
            
            next_write_pos%=_capacity;
            _write_pos.store(next_write_pos, std::memory_order_release);
            return; 
        }

        bool pop(T& value){
            auto read_pos = _read_pos.load(std::memory_order_relaxed);
            if (read_pos == _write_pos_cache) {
                _write_pos_cache = _write_pos.load(std::memory_order_acquire);
                if (read_pos == _write_pos_cache) {
                    return false;
                }
            }

            value = std::move(_data[read_pos++]);
            read_pos %= _capacity;
            _read_pos.store(read_pos, std::memory_order_release);
            return true;
        }


    private:
        std::uint64_t _capacity;
        std::atomic<std::uint64_t> _read_pos{0};
        std::uint64_t _read_pos_cache{0};
        std::atomic<std::uint64_t> _write_pos{0};
        std::uint64_t _write_pos_cache{0};
        std::vector<T> _data;
};