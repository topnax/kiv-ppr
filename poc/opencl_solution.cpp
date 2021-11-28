//
// Created by topnax on 23.11.21.
//




#include <iostream>
#include "opencl_solution.h"
#include "opencl_utils.h"
#include "opencl_bucketing.h"
#include "bucketing_utils.h"


solution_result process_file_opencl(char *file_name, int percentile, const std::string &device_name) {
    // get OpenCL device
    auto device = cl_get_device(device_name);
    // prepare a context
    auto context = cl::Context(device);

    auto result = create_buckets_opencl(file_name, context, device);

    auto perc_pos = find_percentile_position(percentile, result.first, result.second);

    auto solution_result = find_percentile_value_opencl(perc_pos.second, perc_pos.first, file_name,  context, device);

    return solution_result;
}
