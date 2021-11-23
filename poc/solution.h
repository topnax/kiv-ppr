//
// Created by topnax on 23.11.21.
//
#pragma once

#include <utility>
#include <cstdint>

using solution_result = std::pair<double, std::pair<uint64_t, uint64_t>>;

enum solution_mode {
    SERIAL,
    SMP,
    OPENCL,
    NONE
};

solution_result process_for_solution(char *file_name, int percentile, solution_mode mode);

void print_solution_result(solution_result result);
