//
// Created by topnax on 20.11.21.
//
#pragma once

#include <utility>
#include <vector>
#include <cstdint>
#include <cmath>
#include <map>
#include <memory>


struct bucket_item {
    uint64_t count;
    uint64_t lowest_index;
    uint64_t highest_index;
};

std::pair<uint64_t, uint64_t> find_percentile_position(int percentile, std::vector<uint64_t> buckets, uint64_t buckets_total_items);

std::pair<uint64_t, uint64_t>
find_percentile_position_in_subbucket(uint64_t percentile_pos_in_subbucket, std::vector<uint64_t> buckets,
                                      uint64_t base_bucket);

std::pair<double, std::pair<uint64_t, uint64_t>>  find_percentile_in_histogram(uint64_t  percentile_position_in_bucket, std::map<double, std::unique_ptr<bucket_item>> &numbers_in_bucket);

void update_histogram_item(uint64_t &content, std::map<double, std::unique_ptr<bucket_item>> &numbers_in_bucket, uint64_t index);
