cmake_minimum_required(VERSION 3.16)
project(poc)

set(CMAKE_CXX_STANDARD 17)

add_executable(poc
        main.cpp
        serial_bucketing.h
        serial_bucketing.cpp
        smp_solution.cpp
        smp_solution.h
        smp_bucketing.h
        smp_bucketing.cpp
        double_utils.h
        bucketing_utils.cpp
        bucketing_utils.h
        bucketing_constants.h
        serial_solution.h
        serial_solution.cpp
        solution.h
        solution.cpp
        opencl/opencl_sources.h
        opencl_solution.h
        opencl_solution.cpp)

add_executable(verifier verifier.cpp)

target_link_libraries(poc "-ltbb -lOpenCL")

