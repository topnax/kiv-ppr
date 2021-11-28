//
// Created by topnax on 23.11.21.
//
#pragma once

#include <utility>
#include <cstdint>
#include "params.h"

using solution_result = std::pair<double, std::pair<uint64_t, uint64_t>>;

solution_result process_for_solution(parameters params);

void print_solution_result(solution_result result);
