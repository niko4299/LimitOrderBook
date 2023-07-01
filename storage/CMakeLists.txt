set(storage_source_files
rbtree.hpp
)

add_library(storage STATIC ${storage_source_files})
set_target_properties(storage PROPERTIES LINKER_LANGUAGE CXX)

