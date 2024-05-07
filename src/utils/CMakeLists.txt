set(utils_files
    defer.hpp
    file_utils.hpp
    function_task.hpp
    ringbuffer.hpp
    seastar_signal_catcher.hpp
    thread_pool.hpp
    time.hpp
    uuid_generator.hpp
)

add_library(utils INTERFACE ${uuid_generator_src})
target_link_libraries(utils INTERFACE uuid) 
