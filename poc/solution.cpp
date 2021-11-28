//
// Created by topnax on 23.11.21.
//

#include <iostream>
#include "solution.h"
#include "serial_solution.h"
#include "smp_solution.h"
#include "opencl_solution.h"

solution_result process_for_solution(char *file_name, int percentile, solution_mode mode) {
    if (mode == SERIAL) {
        return process_file_serial(file_name, percentile);
    } else if (mode == SMP) {
        return process_file_smp(file_name, percentile);
    } else if (mode == OPENCL) {
        // TODO opencl device name from program arguments (RX480)
        return process_file_opencl(file_name, percentile, "Ellesmere");
    } else {
        std::wcout << "SOLUTION MODE NOT SUPPORTED!" << std::endl;
        exit(-1);
    }
}

void print_solution_result(solution_result result) {
    std::wcout << std::hexfloat << result.first << " " << result.second.first << " " << result.second.second << std::endl;
}
