cmake_minimum_required(VERSION 3.17)
project(PPR)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

include_directories(
        "C:/Program Files (x86)/Intel/oneAPI/tbb/2021.4.0/include"
        "C:/Program Files (x86)/IntelSWTools/system_studio_2020/OpenCL/sdk/include"
)
link_directories(
        "C:/Program Files (x86)/Intel/oneAPI/tbb/2021.4.0/lib/intel64/vc14"
        "C:/Program Files (x86)/IntelSWTools/system_studio_2020/OpenCL/sdk/lib/x64"
)

add_executable(PPR
        src/parameters.cpp
        src/parameters.h
        src/main.cpp
        src/buckets.cpp
        src/buckets.h
        src/naive.cpp
        src/naive.h
        src/utils.cpp
        src/utils.h
        src/percentile_finder.cpp
        src/percentile_finder.h
        src/histogram.cpp
        src/histogram.h
        src/test.cpp
        src/test.h
        src/buckets_single.cpp
        src/buckets_single.h
        src/buckets_smp.cpp
        src/buckets_smp.h
        src/logging.h
        src/watchdog.cpp
        src/watchdog.h
        src/buckets_cl.cpp
        src/buckets_cl.h
        src/cl_lib.h
        src/opencl_src.h)

