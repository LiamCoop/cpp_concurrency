cmake_minimum_required(VERSION 3.16)
project(cpp_concurrency_LiamCoopR)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG true)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)

set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

include_directories(include)
find_package(Threads REQUIRED)

#set(QUEUE_SOURCE app/test_queue.cpp)
set(THREAD_SOURCE lib/thread_pool.cpp app/test_thread_pool.cpp)
set(JULIA_SOURCE lib/thread_pool.cpp app/test_julia_set.cpp)

#add_executable(test_queue ${QUEUE_SOURCE})
target_link_libraries(test_queue Threads::Threads)
#add_executable(test_thread_pool ${THREAD_SOURCE})
target_link_libraries(test_thread_pool Threads::Threads)

set(Boost_USE_MULTITHREADED FALSE)
find_package(Boost 1.72.0 COMPONENTS)
if(Boost_FOUND)
  include_directories(${Boost_LIBRARIES})
  #add_executable(test_julia_set ${JULIA_SOURCE})
  target_link_libraries(test_julia_set Threads::Threads)
endif()
