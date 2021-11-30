//
// Created by topnax on 23.11.21.
//
#pragma once

#include <utility>
#include <cstdint>
#include "params.h"

// solution pair result alias
using solution_result = std::pair<double, std::pair<uint64_t, uint64_t>>;

/**
 * Process the given parameters for a solution
 * @param params parameters of the processing
 * @return a solution result
 */
solution_result process_for_solution(parameters params);

/**
 * Print the solution result
 *
 * @param result result to be printed
 */
void print_solution_result(solution_result result);
