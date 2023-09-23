set(repository_source_files
order_repository.cpp
trade_repository.cpp
)

add_library(repository STATIC ${repository_source_files})
target_link_libraries(repository rocksdb Boost::serialization ${PROJECT_BINARY_DIR}/scylladriver/src/scylladriver-build/libscylla-cpp-driver.so)
