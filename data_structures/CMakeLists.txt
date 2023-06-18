set(datastructure_source_files
limit.cpp
order.cpp
rbtree.hpp
trade.hpp
enums.hpp
)

add_library(data_structures SHARED ${datastructure_source_files})
