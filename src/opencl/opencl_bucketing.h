//
// Created by topnax on 10.10.21.
//
#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include "../bucketing/bucketing_utils.h"
#include "../solution.h"
#include "opencl_utils.h"

/**
 * Create buckets based on the file's content using the OpenCL device
 *
 * @param file_name name of the file to be processed
 * @param context OpenCL context
 * @param dev OpenCL device to used for processing
 *
 * @return array representing the bucket histogram and the number of items present in buckets
 */
std::pair<std::vector<uint64_t>, uint64_t> create_buckets_opencl(char *file_name, cl::Context &context, cl::Device &dev);
