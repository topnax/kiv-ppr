//
// Created by topnax on 23.11.21.
//

#include <iostream>
#include "solution.h"
#include "single/single_solution.h"
#include "smp/smp_solution.h"
#include "opencl/opencl_solution.h"
#include "params.h"

solution_result process_for_solution(parameters params) {
    auto file_name = params.file_name.data();
    auto percentile = params.percentile;
    if (params.mode == solution_mode::SINGLE) {
        return process_file_single(file_name, percentile);
    } else if (params.mode == solution_mode::SMP) {
        return process_file_smp(file_name, percentile);
    } else if (params.mode == solution_mode::OPENCL) {
        return process_file_opencl(file_name, percentile, params.device);
    } else {
        std::wcout << "SOLUTION MODE NOT SUPPORTED!" << std::endl;
        exit(-1);
    }
}

void print_solution_result(solution_result result) {
    std::wcout << std::hexfloat << result.first << " " << result.second.first << " " << result.second.second << std::endl;
}
