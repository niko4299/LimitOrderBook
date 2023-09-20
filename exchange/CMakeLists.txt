set(exchange_source_files
exchange.cpp
)

add_library(exchange STATIC ${exchange_source_files})
target_link_libraries(exchange data_structures)