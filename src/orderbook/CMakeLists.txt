set(orderbook_source_files
orderbook.cpp
)

add_library(orderbook STATIC ${orderbook_source_files})
target_link_libraries(orderbook data_structures)
