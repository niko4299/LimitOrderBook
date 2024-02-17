#pragma once

#include <seastar/http/httpd.hh>

static inline const seastar::sstring INSTRUMENT_KEY = seastar::sstring("instrument");
static inline const seastar::sstring ORDER_ID_KEY = seastar::sstring("order_id");
