set(repository_source_files
server.cpp
)

add_library(server STATIC ${repository_source_files})
target_link_libraries(server exchange seastar)
