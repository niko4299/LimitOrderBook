#include <seastar/core/print.hh>
#include <seastar/http/httpd.hh>
#include <seastar/json/json_elements.hh>

#include "../../../data_structures/snapshot.hpp"

struct SeastarSpreadJson : public seastar::json::json_base {
    seastar::json::json_element<float> best_ask_price;
    seastar::json::json_element<float> market_price;
    seastar::json::json_element<float> best_bid_price;

    void register_params() {
        add(&best_ask_price, "best_ask_price");
        add(&market_price, "market_price");
        add(&best_bid_price, "best_bid_price");
    }

    SeastarSpreadJson() {
        register_params();
    }

    SeastarSpreadJson(const SeastarSpreadJson& e) : seastar::json::json_base() {
        register_params();
        best_ask_price = e.best_ask_price;
        market_price = e.market_price;
        best_bid_price = e.best_bid_price;
    }

    template<class T>
    SeastarSpreadJson& operator=(const T& e){
        best_ask_price = e.best_ask_price;
        market_price = e.market_price;
        best_bid_price = e.best_bid_price;

        return *this;
    }


    SeastarSpreadJson(float best_ask_price, float market_price, float best_bid_price) {
        register_params();

        this->best_ask_price = best_ask_price;
        this->market_price = market_price;
        this->best_bid_price = best_bid_price;
    }


    SeastarSpreadJson& operator=(const SeastarSpreadJson& e){
        best_ask_price = e.best_ask_price;
        market_price = e.market_price;
        best_bid_price = e.best_bid_price;

        return *this;
    }

};

struct SeastarLimitJson : public seastar::json::json_base {
    seastar::json::json_element<float> price;
    seastar::json::json_element<float> qty;

    void register_params() {
        add(&price, "price");
        add(&qty, "qty");
    }
    
    SeastarLimitJson() {
        register_params();
    }


    SeastarLimitJson(const SeastarLimitJson& e) : seastar::json::json_base() {
        register_params();
        price = e.price;
        qty = e.qty;
    }

    template<class T>
    SeastarLimitJson& operator=(const T& e){
        price = e.price;
        qty = e.qty;

        return *this;
    }

    SeastarLimitJson(float price, float qty) {
        register_params();

        this->price = price;
        this->qty = qty;
    }  

    SeastarLimitJson& operator=(const SeastarLimitJson& e){
        price = e.price;
        qty = e.qty;

        return *this;
    }
};

struct SeastarSnapshotJson : public seastar::json::json_base {
    seastar::json::json_element<std::vector<SeastarLimitJson>> asks;
    seastar::json::json_element<std::vector<SeastarLimitJson>> bids;
    seastar::json::json_element<SeastarSpreadJson> spread;

    void register_params() {
        add(&asks, "asks");
        add(&bids, "bids");
        add(&spread, "spread");
    }

    SeastarSnapshotJson() {
        register_params();
    }

    SeastarSnapshotJson(const SeastarSnapshotJson& e) : seastar::json::json_base() {
        register_params();
        asks = e.asks;
        bids = e.bids;
        spread = e.spread;
    }

    template<class T>
    SeastarSnapshotJson& operator=(const T& e){
        asks = e.asks;
        bids = e.bids;
        spread = e.spread;

        return *this;
    }

    SeastarSnapshotJson(Snapshot snapshot) {
        register_params();
        std::vector<SeastarLimitJson> temp_asks{};
        std::vector<SeastarLimitJson> temp_bids{};

        for(auto limit: snapshot.asks){
            temp_asks.emplace_back(SeastarLimitJson(limit.price, limit.qty));
        }

        for(auto limit: snapshot.bids){
            temp_bids.emplace_back(SeastarLimitJson(limit.price, limit.qty));
        }

        this->asks = temp_asks;
        this->bids = temp_bids;
        this->spread = SeastarSpreadJson(snapshot.spread.best_ask_price, snapshot.spread.market_price, snapshot.spread.best_bid_price);
    }

    SeastarSnapshotJson& operator=(const SeastarSnapshotJson& e){
        asks = e.asks;
        bids = e.bids;
        spread = e.spread;

        return *this;
    }
};
