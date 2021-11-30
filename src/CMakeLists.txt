cmake_minimum_required(VERSION 3.16)
project(pprsolver)

set(CMAKE_CXX_STANDARD 17)

add_executable(pprsolver
        main.cpp
        single/single_bucketing.h
        single/single_bucketing.cpp
        smp/smp_solution.cpp
        smp/smp_solution.h
        smp/smp_bucketing.h
        smp/smp_bucketing.cpp
        utils/double_utils.h
        bucketing/bucketing_utils.cpp
        bucketing/bucketing_utils.h
        bucketing/bucketing_constants.h
        single/single_solution.h
        single/single_solution.cpp
        solution.h
        solution.cpp
        opencl/opencl_sources.h
        opencl/opencl_solution.h
        opencl/opencl_bucketing.h
        opencl/opencl_bucketing.cpp
        opencl/opencl_solution.h
        opencl/opencl_solution.cpp
        opencl/opencl_utils.cpp
        opencl/opencl_utils.h
        params.h
        params.cpp
        utils/watchdog.h
        utils/watchdog.cpp
        )

add_executable(naivepprsolver naive/naive.cpp)

target_link_libraries(pprsolver "-ltbb -lOpenCL -lpthread")

