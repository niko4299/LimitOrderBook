#include <default_wrapper.hpp>

CoinBaseWrapper::DefaultWrapper(std::unique_ptr<Exchange> exchange): {
    _exchange = exchange;
}


OrderStatus CoinBaseWrapper::process_message(std::string& message){
    
    _exchange->create_order(message);

    return 

}