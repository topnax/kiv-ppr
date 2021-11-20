cmake_minimum_required(VERSION 3.17)
project(poc)

set(CMAKE_CXX_STANDARD 17)

add_executable(poc
        main.cpp
        bucketing.h
        bucketing.cpp
        BufferedReader.cpp
        BufferedReader.h
        smp_solution.cpp
        smp_solution.h
        smp_bucketing.h
        smp_bucketing.cpp
        double_utils.h
        bucketing_utils.cpp
        bucketing_utils.h
        bucketing_constants.h
        )

target_link_libraries(poc "-ltbb")

add_executable(verifier verifier.cpp)

add_executable(test test.cpp bucketing.h BufferedReader.h BufferedReader.cpp)
