//
// Created by topnax on 23.11.21.
//

#include <iostream>
#include "solution.h"
#include "serial_solution.h"
#include "smp_solution.h"
#include "opencl_solution.h"
#include "params.h"

solution_result process_for_solution(parameters params) {
    std::wcout << params.percentile << std::endl;
    std::wcout << params.file_name.data() << std::endl;
    auto file_name = params.file_name.data();
    auto percentile = params.percentile;
    if (params.mode == SERIAL) {
        return process_file_serial(file_name, percentile);
    } else if (params.mode == SMP) {
        return process_file_smp(file_name, percentile);
    } else if (params.mode == OPENCL) {
        return process_file_opencl(file_name, percentile, params.device);
    } else {
        std::wcout << "SOLUTION MODE NOT SUPPORTED!" << std::endl;
        exit(-1);
    }
}

void print_solution_result(solution_result result) {
    std::wcout << std::hexfloat << result.first << " " << result.second.first << " " << result.second.second << std::endl;
}
