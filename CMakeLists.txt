cmake_minimum_required(VERSION 2.8)

project(infinity)

find_package(Threads REQUIRED)

set (CMAKE_CXX_FLAGS "-fPIC -Wall")

include_directories(${PROJECT_SOURCE_DIR})

add_executable(infinity infinity.cpp)

target_link_libraries(infinity ${CMAKE_THREAD_LIBS_INIT})
