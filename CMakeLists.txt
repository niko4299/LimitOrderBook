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

add_subdirectory(src/storage)
add_subdirectory(src/data_structures)
add_subdirectory(src/exchange)
add_subdirectory(src/orderbook)
add_subdirectory(src/repository)
add_subdirectory(src/server)
add_subdirectory(tests)

set(ROCKSDB_ROOT ${PROJECT_BINARY_DIR}/rocksdb)
ExternalProject_Add(
    rocksdb
    PREFIX            ${ROCKSDB_ROOT}
    INSTALL_DIR       ${ROCKSDB_ROOT}
    GIT_REPOSITORY "https://github.com/facebook/rocksdb.git"
    GIT_TAG v8.5.3
)
set(ROCKSDB_INCLUDE_DIR ${ROCKSDB_ROOT}/src/rocksdb/include)

set(SCYLLA_DRIVER_ROOT ${PROJECT_BINARY_DIR}/scylladriver)
ExternalProject_Add(
    scylladriver
    PREFIX            ${SCYLLA_DRIVER_ROOT}
    INSTALL_DIR       ${SCYLLA_DRIVER_ROOT}
    GIT_REPOSITORY "https://github.com/scylladb/cpp-driver.git"
)

set(SCYLLA_DRIVER_INCLUDE_DIR ${SCYLLA_DRIVER_ROOT}/src/scylladriver/include)
set(SCYLLA_DRIVER_LIBRARIES ${SCYLLA_DRIVER_ROOT}/src/scylladriver-build/libscylla-cpp-driver.so)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)

set(Seastar_TESTING OFF CACHE BOOL "" FORCE)
set(Seastar_API_LEVEL 6 CACHE STRING "" FORCE)
set(Seastar_CXX_FLAGS -Wno-error)

FetchContent_Declare(
    seastar
    GIT_REPOSITORY https://github.com/scylladb/seastar
    GIT_SHALLOW ON
    GIT_SUBMODULES ""
    GIT_PROGRESS TRUE
    USES_TERMINAL_DOWNLOAD TRUE
    OVERRIDE_FIND_PACKAGE
    SYSTEM
    PATCH_COMMAND sed -i "s/add_subdirectory (tests/# add_subdirectory (tests/g" CMakeLists.txt
)

FetchContent_Declare(
    simdjson
    GIT_REPOSITORY "https://github.com/simdjson/simdjson"
    GIT_TAG v3.6.4
)


FetchContent_MakeAvailable(seastar googletest simdjson)
enable_testing()

include_directories(ROCKSDB_INCLUDE_DIR)
include_directories(SCYLLA_DRIVER_INCLUDE_DIR)
add_executable(UnlimitedOrderBook src/main.cpp)
add_dependencies(UnlimitedOrderBook rocksdb scylladriver)
target_link_libraries(UnlimitedOrderBook PRIVATE data_structures storage repository orderbook exchange server seastar simdjson gflags)
