#pragma once

#include "spread.hpp"

struct SnapshotLimit{
    float price;
    float qty;
};

struct Snapshot{
    std::vector<SnapshotLimit> asks;
    std::vector<SnapshotLimit> bids;
    Spread spread;
};
