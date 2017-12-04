cmake_minimum_required(VERSION 3.0)

project(app LANGUAGES CXX)

find_package(Threads REQUIRED)

add_executable(app infinity.cpp)

target_link_libraries(app ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(app clog)
target_link_libraries(app Node)
