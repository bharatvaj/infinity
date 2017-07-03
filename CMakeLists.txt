cmake_minimum_required(VERSION 2.8)

project(ftp_cry)

find_package(Threads REQUIRED)

set (CMAKE_CXX_FLAGS "-fPIC -Wall")

include_directories(${PROJECT_SOURCE_DIR})

add_executable(ftp_cry ftp_cry.cpp)

target_link_libraries(ftp_cry ${CMAKE_THREAD_LIBS_INIT})
