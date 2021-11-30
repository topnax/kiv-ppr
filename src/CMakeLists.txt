cmake_minimum_required(VERSION 3.16)
project(pprsolver)

set(CMAKE_CXX_STANDARD 17)

add_executable(pprsolver
        main.cpp
        single_bucketing.h
        single_bucketing.cpp
        smp_solution.cpp
        smp_solution.h
        smp_bucketing.h
        smp_bucketing.cpp
        double_utils.h
        bucketing_utils.cpp
        bucketing_utils.h
        bucketing_constants.h
        single_solution.h
        single_solution.cpp
        solution.h
        solution.cpp
        opencl/opencl_sources.h
        opencl_solution.h
        opencl_bucketing.h
        opencl_bucketing.cpp
        opencl_solution.h
        opencl_solution.cpp
        opencl_utils.cpp
        opencl_utils.h
        params.h
        params.cpp
        watchdog.h
        watchdog.cpp
        )

add_executable(naive naive.cpp)

target_link_libraries(pprsolver "-ltbb -lOpenCL -lpthread")

