#include <exchange/exchange.hpp>

class Wrapper {
    public:
        virtual OrderStatus process_message(std::string& message) = 0;
}