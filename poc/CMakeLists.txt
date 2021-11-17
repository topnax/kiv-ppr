cmake_minimum_required(VERSION 3.17)
project(poc)

set(CMAKE_CXX_STANDARD 17)

add_executable(poc main.cpp bucketing.h bucketing.cpp BufferedReader.cpp BufferedReader.h)

add_executable(verifier verifier.cpp )

add_executable(test test.cpp bucketing.h BufferedReader.h BufferedReader.cpp)
