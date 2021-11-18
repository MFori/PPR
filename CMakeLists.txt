cmake_minimum_required(VERSION 3.17)
project(PPR)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(PPR
        src/parameters.cpp src/parameters.h src/main.cpp src/buckets.cpp src/buckets.h src/naive.cpp src/naive.h src/utils.cpp src/utils.h)
