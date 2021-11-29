//
// Created by topnax on 20.11.21.
//
#pragma once

#include <vector>
#include <cstdint>

using buckets_and_item_count = std::pair<std::vector<uint64_t>, uint64_t>;

buckets_and_item_count create_buckets_smp(char *file_name);

std::pair<double, std::pair<uint64_t, uint64_t>> find_percentile_value_smp(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name);
