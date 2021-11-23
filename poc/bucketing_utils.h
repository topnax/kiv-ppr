//
// Created by topnax on 20.11.21.
//
#pragma once

#include <utility>
#include <vector>
#include <cstdint>
#include <cmath>


std::pair<uint64_t, uint64_t> find_percentile_position(int percentile, std::vector<uint64_t> buckets, uint64_t buckets_total_items);

std::pair<uint64_t, uint64_t>
find_percentile_position_in_subbucket(uint64_t percentile_pos_in_subbucket, std::vector<uint64_t> buckets,
                                      uint64_t base_bucket);
