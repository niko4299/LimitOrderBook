#pragma once

enum class Side { UNKOWN, BUY, SELL };

enum class OrderParams : std::uint64_t {
    STOP = 1,        // stop limit order
    AON = 1U << 1U,  // all-or-nothing
    IOC = 1U << 2U,  // immediate-or-cancel
    FOK = AON | IOC, // fill-or-kill
    GTC = 1U << 3U,  // good-till-cancelled
    GFD = 1U << 4U,  // good-for-day
    GTD = 1U << 5U,  // good-till-date
};

enum class OrderType { MARKET, LIMIT };

enum class OrderStatus{ACCEPTED, MATCHED, NOT_MATCHED, REJECTED, CANCELLED, MODIFIED, NOT_FOUND};
