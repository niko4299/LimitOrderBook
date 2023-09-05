set(test_prefix unit_)
add_custom_target(unit)

find_package(fmt REQUIRED)
find_package(gflags REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.74.0 COMPONENTS system filesystem serialization REQUIRED)

add_executable(test_rbtree test_rbtree.cpp)
add_test(${test_prefix}test_rbtree test_rbtree)
target_link_libraries(test_rbtree storage gtest gmock)

add_executable(test_order test_order.cpp)
add_test(${test_prefix}test_order test_order)
target_link_libraries(test_order data_structures gtest gmock Boost::serialization)

add_executable(test_limit test_limit.cpp)
add_test(${test_prefix}test_limit test_limit)
target_link_libraries(test_limit data_structures gtest gmock Boost::serialization)

add_executable(test_order_repository test_order_repository.cpp)
add_test(${test_prefix}test_order_repository test_order_repository)
target_link_libraries(test_order_repository repository rocksdb data_structures gtest gmock Boost::serialization)

add_executable(test_orderbook test_orderbook.cpp)
add_test(${test_prefix}test_orderbook test_orderbook)
target_link_libraries(test_orderbook orderbook repository rocksdb data_structures gtest gmock Boost::serialization)

add_executable(test_trade_repository test_trade_repository.cpp)
add_test(${test_prefix}test_trade_repository test_trade_repository)
target_link_libraries(test_trade_repository repository data_structures gtest gmock ${PROJECT_BINARY_DIR}/scylladriver/src/scylladriver-build/libscylla-cpp-driver.so)
