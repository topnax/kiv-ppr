//
// Created by topnax on 23.11.21.
//
#include "opencl_solution.h"
#include "opencl_utils.h"
#include "opencl_bucketing.h"
#include "serial_bucketing.h"
#include "bucketing_utils.h"


solution_result process_file_opencl(char *file_name, int percentile, cl::Device &device) {
    // prepare a context
    auto context = cl::Context(device);

    // create buckets
    auto result = create_buckets_opencl(file_name, context, device);

    // find position of the percentile in the bucket
    auto perc_pos = find_percentile_position(percentile, result.first, result.second);

    // find the percentile value in the bucket that was found
    auto solution_result = find_percentile_value_serial(perc_pos.second, perc_pos.first, file_name);

    return solution_result;
}
