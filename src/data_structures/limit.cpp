#include "limit.hpp"
#include "order.hpp"

Limit::Limit(float price): _price{price} {}

float Limit::get_price() {
    return _price;
}

void Limit::increase_number_of_orders() {
    _number_of_active_orders++;
    _total_number_of_orders++;
}

void Limit::decrease_number_of_orders() {
    _number_of_active_orders--;
}

std::uint64_t Limit::get_number_of_active_orders() {
    return _number_of_active_orders;
}

std::uint64_t Limit::get_total_number_of_orders() {
    return _total_number_of_orders;
}

void Limit::increase_volume(float delta_qty) {
    _active_volume += delta_qty;
    _total_volume += delta_qty;
}

void Limit::decrease_volume(float delta_qty) {
    _active_volume = std::max(0.0, _active_volume - delta_qty);
}

float Limit::get_active_volume() {
    return _active_volume;
}

float Limit::get_total_volume() {
    return _total_volume;
}

std::vector<std::shared_ptr<Order>> Limit::all_active_orders() {
    std::vector<std::shared_ptr<Order>> active_orders{};

    auto curr = _head;
    while (curr) {
        if (!curr->is_fullfilled() && !curr->is_cancelled()) {
            active_orders.push_back(curr);
        }

        curr = curr->_next;
    }

    return active_orders;
}

bool Limit::empty() {
    return !_head && !_tail;
}

Side Limit::side() {
    if (empty()) {
        return Side::UNKOWN;
    }

    return _head->is_buy() ? Side::BUY : Side::SELL;
}

// bool Limit::operator==(const Limit& other) const {
//     return _price == other._price;
// }

// bool Limit::operator==(const std::shared_ptr<Limit>& other) const {
//     return _price == other->get_price();
// }

// bool Limit::operator>(const std::shared_ptr<Limit>& other) const {
//     return _price > other->get_price();
// }

template<class Archive>
void Limit::serialize(Archive &a, const unsigned version){
    a & _price & _number_of_active_orders & _total_number_of_orders & _active_volume & _total_volume;

    auto& curr = _head;
    while(curr){
        a & curr;
        curr = curr->_next;
    }
}

void Limit::clear() {
    _head = _tail = nullptr;
}
