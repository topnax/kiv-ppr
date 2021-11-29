//
// Created by topnax on 10.10.21.
//
#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include "bucketing_utils.h"
#include "solution.h"
#include "opencl_utils.h"

std::pair<std::vector<uint64_t>, uint64_t> create_buckets_opencl(char *file_name, cl::Context &context, cl::Device &dev);

std::pair<double, std::pair<uint64_t, uint64_t>>
find_percentile_value_opencl(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name, cl::Context &context, cl::Device &dev);
