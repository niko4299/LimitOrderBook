#pragma once

#include "spread.hpp"

struct Snapshot{
    std::vector<std::shared_ptr<Order>> asks;
    std::vector<std::shared_ptr<Order>> bids;
    Spread spread;
};
