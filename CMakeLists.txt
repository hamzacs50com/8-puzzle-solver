cmake_minimum_required(VERSION 3.10)
project(8_Puzzle_Solver VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

add_executable(solver src/main.cpp)
