//
// Created by topnax on 10.10.21.
//

#include <vector>
#include <cmath>
#include <cstdint>
#include "bucketing_utils.h"

#ifndef POC_BUCKETING_H
#define POC_BUCKETING_H


std::pair<std::vector<uint64_t>, uint64_t> create_buckets(char *file_name);

std::pair<std::vector<uint64_t>, uint64_t> create_sub_buckets(char *file_name, uint64_t base_bucket_index);

std::pair<double, std::pair<uint64_t, uint64_t>>
find_percentile_value(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name);

solution_result
find_percentile_value_subbucket(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name, uint64_t subbucket);

#endif //POC_BUCKETING_H
