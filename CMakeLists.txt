cmake_minimum_required(VERSION 3.17)
project(PPR)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

include_directories("C:/Program Files (x86)/Intel/oneAPI/tbb/2021.4.0/include")
link_directories("C:/Program Files (x86)/Intel/oneAPI/tbb/2021.4.0/lib/intel64/vc14")

add_executable(PPR
        src/parameters.cpp src/parameters.h src/main.cpp src/buckets.cpp src/buckets.h src/naive.cpp src/naive.h src/utils.cpp src/utils.h src/percentile_finder.cpp src/percentile_finder.h src/state.cpp src/state.h src/histogram.cpp src/histogram.h src/test.cpp src/test.h src/smp_file_reader.cpp src/smp_file_reader.h src/buckets_single.cpp src/buckets_single.h src/buckets_smp.cpp src/buckets_smp.h)

