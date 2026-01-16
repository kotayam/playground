cmake_minimum_required(VERSION 3.14)
project(ConcurrencySamples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(lock_guard lock_guard.cc)
target_link_libraries(lock_guard PRIVATE Threads::Threads)

add_executable(unique_lock unique_lock.cc)
target_link_libraries(unique_lock PRIVATE Threads::Threads)

add_executable(condition_variable condition_variable.cc)
target_link_libraries(condition_variable PRIVATE Threads::Threads)
