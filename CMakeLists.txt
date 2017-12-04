cmake_minimum_required(VERSION 3.0)
project(infinity)

set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-Wall")

option(ENABLE_CLOG "Enables logging" ON)

#clog
add_subdirectory(clog ${CMAKE_SOURCE_DIR}/build/clog)

#Node
add_subdirectory(Node ${CMAKE_SOURCE_DIR}/build/Node)

#app
add_subdirectory(app ${CMAKE_SOURCE_DIR}/build/app)
