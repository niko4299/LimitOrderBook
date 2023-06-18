set(repository_source_files
order_repository.cpp
trade_repository.cpp
)

add_library(repository SHARED ${repository_source_files})
