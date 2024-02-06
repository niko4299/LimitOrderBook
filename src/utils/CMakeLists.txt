set(utils_files
    defer.hpp
    function_task.hpp
    ringbuffer.hpp
    seastar_signal_catcher.hpp
    thread_pool.hpp
    uuid_generator.hpp
    file_utils.hpp)

add_library(utils INTERFACE ${uuid_generator_src})
target_link_libraries(utils INTERFACE uuid) 
