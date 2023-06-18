set(repository_source_files
websocket_server.cpp
websocket_client.cpp
)

add_library(server SHARED ${repository_source_files})
