cmake_minimum_required(VERSION 3.10)
project(libfasttouch C CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

add_definitions(-DBUILD_FOR_LINUX)
add_definitions(-DCORE_TEENSY)
add_library(fasttouch FastTouch.cpp)

