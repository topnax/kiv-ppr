//
// Created by topnax on 23.11.21.
//




#include <iostream>
#include "bucketing_constants.h"
#include "opencl_solution.h"
#include "opencl_utils.h"
#include "opencl_bucketing.h"
#include "bucketing_utils.h"


solution_result process_file_opencl(char *file_name, int percentile) {
    // get OpenCL device
    auto device = cl_get_gpu_device();
    // prepare a context
    auto context = cl::Context(device);

    auto result = create_buckets_opencl(file_name, context, device);

    auto perc_pos = find_percentile_position(percentile, result.first, result.second);
    std::wcout
            << "first: " << perc_pos.first << std::endl
            << "second: " << perc_pos.second << std::endl;

    return solution_result(1, std::pair(1, 1));
}
