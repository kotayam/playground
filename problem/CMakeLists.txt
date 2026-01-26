cmake_minimum_required(VERSION 3.14)
project(Problems)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(id_allocator id_allocator.cc)

add_executable(snapshot_map snapshot_map.cc)

add_executable(ttl_cache ttl_cache.cc)
target_link_libraries(ttl_cache PRIVATE Threads::Threads)

