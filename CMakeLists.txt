cmake_minimum_required(VERSION 3.22)
project(UnlimitedOrderBook)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compile flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall \
    -Werror=switch -Werror=switch-bool")

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -pthread")

include(FetchContent)
include(ExternalProject)
find_package(Boost 1.74.0 COMPONENTS system filesystem serialization thread REQUIRED)

add_subdirectory(storage)
add_subdirectory(data_structures)
add_subdirectory(orderbook)
add_subdirectory(repository)
add_subdirectory(server)
add_subdirectory(tests)

set(ROCKSDB_ROOT ${PROJECT_BINARY_DIR}/rocksdb)
ExternalProject_Add(
    rocksdb
    PREFIX            ${ROCKSDB_ROOT}
    INSTALL_DIR       ${ROCKSDB_ROOT}
    GIT_REPOSITORY "https://github.com/facebook/rocksdb.git"
    GIT_TAG v8.1.1
)

set(SCYLLA_DRIVER_ROOT ${PROJECT_BINARY_DIR}/scylladriver)
ExternalProject_Add(
    scylladriver
    PREFIX            ${SCYLLA_DRIVER_ROOT}
    INSTALL_DIR       ${SCYLLA_DRIVER_ROOT}
    GIT_REPOSITORY "https://github.com/scylladb/cpp-driver.git"
)

enable_testing()

set(ROCKSDB_INCLUDE_DIR ${ROCKSDB_ROOT}/src/rocksdb/include)
set(SCYLLA_DRIVER_INCLUDE_DIR ${SCYLLA_DRIVER_ROOT}/src/scylladriver/include)
set(SCYLLA_DRIVER_LIBRARIES ${SCYLLA_DRIVER_ROOT}/src/scylladriver-build/libscylla-cpp-driver.so)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)
FetchContent_MakeAvailable(googletest)

include_directories(ROCKSDB_INCLUDE_DIR)
include_directories(SCYLLA_DRIVER_INCLUDE_DIR)
add_executable(UnlimitedOrderBook main.cpp)
add_dependencies(UnlimitedOrderBook rocksdb scylladriver)
target_link_libraries(UnlimitedOrderBook PRIVATE data_structures storage repository orderbook server Boost::boost Boost::thread dl ${SCYLLA_DRIVER_LIBRARIES})
