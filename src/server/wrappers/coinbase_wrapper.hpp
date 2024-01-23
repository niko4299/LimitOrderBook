#include <wrapper.hpp>

class CoinBaseWrapper: public Wraper{
    
    public:

        explicit CoinBaseWrapper(std::unique_ptr<Exchange>& exchange);

        OrderStatus process_message(std::string& message) override;
    
    private:

        std::unique_ptr<Exchange> _exchange;
}