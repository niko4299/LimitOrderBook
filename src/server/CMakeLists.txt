set(server_source_files
    handlers/order_handler.cpp
    server.cpp
)

add_library(server STATIC ${server_source_files})
target_link_libraries(server exchange seastar simdjson)
