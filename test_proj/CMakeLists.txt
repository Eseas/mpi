cmake_minimum_required(VERSION 3.12)
project(test_proj)

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(test_proj main.cpp)
target_link_libraries(test_proj ${SDL2_LIBRARIES})