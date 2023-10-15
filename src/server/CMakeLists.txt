set(repository_source_files
websocket_server.cpp
websocket_client.cpp
)

add_library(server STATIC ${repository_source_files})
