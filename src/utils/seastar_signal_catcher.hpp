#include <seastar/core/sharded.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/condition-variable.hh>

class StopSignal {
    public:
        StopSignal() {
            seastar::engine().handle_signal(SIGINT, [this] { signaled(); });
            seastar::engine().handle_signal(SIGTERM, [this] { signaled(); });
        }
        ~StopSignal() {
            seastar::engine().handle_signal(SIGINT, [] {});
            seastar::engine().handle_signal(SIGTERM, [] {});
        }
        seastar::future<> wait() {
            return _cond.wait([this] { return _caught; });
        }
        bool stopping() const {
            return _caught;
        }
    private:
        void signaled() {
            if (_caught) {
                return;
            }
            _caught = true;
            _cond.broadcast();
        }

    bool _caught = false;
    seastar::condition_variable _cond;
};
