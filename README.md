# UnlimitedOrderBook

Welcome to UnlimitedOrderBook, a C++ project created based on my findings and personal vision for the implementation of a stock exchange, with a primary focus on OrderBook functionality. This project serves as a platform for my exploration into various concepts, learning C++, and gaining insights into the workings of financial exchanges.

## Currently Supported

### Order Types
- **Market Order:** Execute an order as fast as possible, crossing the spread.
- **Limit Order:** Execute an order with a limit on bid/ask price (e.g., $x or less for a bid, or $y or more for an ask).

### Order Parameters
- **STOP:** Stop order, set a stop price which will activate the order once the market price crosses it.
- **AON (All or Nothing):** Don't allow partial fills.
- **IOC (Immediate or Cancel):** Immediately fill what's possible, cancel the rest.
- **FOK (Fill or Kill):** AON+IOC, immediately match an order in full (without partial fills) or cancel it.

## TODO

* [x] Basic orderbook functionalities
* [x] Add Trade repository and order repository
* [x] Add server functionalities
* [x] GFD, GTC, GTD orders
* [ ] Implement a proper way of doing orderbook snapshots
* [ ] Perform comprehensive benchmarking
* [ ] Add Python bindings
* [ ] Implement risk assessment and limits

## Market Behavior

In the UnlimitedOrderBook, market orders always take precedence over other orders and are sorted based on the time of arrival.

### Order Prioritization and Sorting
- Orders are processed on a first-in-first-out (FIFO) basis.
  - For Bids:
    - Sorted by price (descending) and then by time of arrival (ascending).
  - For Asks:
    - Sorted by price (ascending) and then by time of arrival (ascending).
  - Quantity does not influence the sorting process.

### Market Price Determination
- The market price is set at the last trade price.

### Stop Orders Activation
- Stop Bids are activated when the market price is above or equal to the stop price.
- Stop Asks are activated when the market price is below or equal to the stop price.

### Limit Order Matching
- When a match occurs between two limit orders, the price is set based on the bid price.
  - Example: A Bid of $25 and an Ask of $24 will be matched at $25.

## Architecture 

The exchange contains information about existing instruments, and each instrument has its own orderbook. Each object communicates with others using a lock-free ring buffer.

### Directory information
* **exchange:** Exchange functionalities
* **orderbook:** Orderbook functionalities
* **data_structures:** All data structures used in the project (order, limit, trade...)
* **storage:** RBTree implementation used as in-memory storage
* **server:** Seastar server 
* **order repository:** Persistent storage of orders in RocksDB
* **trade repository:** Persistent storage of trades in ScyllaDB
* **utils:** Lock-free ring buffer implementation, thread pool implementation, UUID generator...

## How to use

1. Start the project by running the following command in the project root:

    ```bash
    docker-compose up -d
    ```

   This will build the UnlimitedOrderBook image, and it may take some time.

2. In the `postman/` directory, you can find a collection of API requests to play around with.

3. For local development, follow these steps:

   - If needed, install dependencies (check them out before running command):

     ```bash
     sudo ./install-dependencies.sh
     ```

   - Start ScyllaDB locally:

     ```bash
      docker run --name trade-repository -d -v $(pwd)/scylla/:/etc/scylla/ scylladb/scylla --smp 1 --memory 750M --overprovisioned 1 --api-address 0.0.0.0
      sudo docker exec -it trade-repository cqlsh -f etc/scylla/trade-repository.txt
     ```
     
    Note: wait for a least 30 sec for scylladb to boot up before running second command.

   - Compile and start the project:

     ```bash
     mkdir -p build
     sudo cmake -DCMAKE_BUILD_TYPE=Release ..
     sudo make -j4 
     ./UnlimitedOrderBook
     ```

    Supported flags are:
     - `--server_name`: Specifies the server name. Default value is "UnlimitedExchange".
     - `--server_address`: Sets the IP address on which the server will listen. Default is "0.0.0.0".
     - `--server_port`: Configures the port on which the server will listen. Default is 8000.
     - `--trade_repository_address`: Defines the IP address of the Scylla database used as the trade repository. Default is "172.17.0.2".
     - `--trade_repository_batch_size`: Sets the batch size for inserting into the trade repository. Currently, only a batch size of 1 is supported, so the value should be set to 1.
     - `--rocksdb_dir`: Specifies the directory for the order RocksDB. Default is "./order_rocksdb".
     - `--ringbuffer_size`: Configures the ringbuffer size. Default is 1024 (same for all instances).
     - `--instrument_init_file`: Sets the path for the initial instrument information file. Default is "../init/instruments.txt".

## Acknowledgements

* [How to Build a Fast Limit Order Book](https://web.archive.org/web/20110219163448/http://howtohft.wordpress.com/2011/02/15/how-to-build-a-fast-limit-order-book/)
* [Basics of Trading Stocks: Know Your Orders](https://www.investopedia.com/investing/basics-trading-stock-know-your-orders/)
* [ScyllaDB](https://github.com/scylladb/scylladb/)
* [Seastar](https://github.com/scylladb/seastar)
* [RocksDB](https://github.com/facebook/rocksdb)
* [simdjson](https://github.com/simdjson/simdjson)
* [stduuid](https://github.com/mariusbancila/stduuid.git)
