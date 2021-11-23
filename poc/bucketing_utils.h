//
// Created by topnax on 20.11.21.
//

#ifndef POC_BUCKETING_UTILS_H
#define POC_BUCKETING_UTILS_H

#include <utility>
#include <vector>
#include <cstdint>
#include <cmath>

using solution_result = std::pair<double, std::pair<uint64_t, uint64_t>>;

std::pair<uint64_t, uint64_t> find_percentile_position(int percentile, std::vector<uint64_t> buckets, uint64_t buckets_total_items);

std::pair<uint64_t, uint64_t>
find_percentile_position_in_subbucket(uint64_t percentile_pos_in_subbucket, std::vector<uint64_t> buckets,
                                      uint64_t base_bucket);

void print_solution_result(solution_result result);

#endif //POC_BUCKETING_UTILS_H
