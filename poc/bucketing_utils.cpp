//
// Created by topnax on 20.11.21.
//

#include <iostream>
#include <ios>
#include "bucketing_utils.h"
#include "bucketing_constants.h"

std::pair<uint64_t , uint64_t > find_percentile_position(int percentile, std::vector<uint64_t > buckets, uint64_t  buckets_total_items) {
    uint64_t percentile_position = (buckets_total_items - 1) * ((double) percentile / (double) 100);

    uint64_t item_count = 0;

    uint64_t bucket_index = buckets.size() - 1;
    bool bucket_found = false;
    // iterate over negative numbers in ascending order
    for (; bucket_index >= buckets.size() / 2; bucket_index--) {
        item_count += buckets[bucket_index];
        if (percentile_position < item_count) {
            bucket_found = true;
            break;
        }
    }

    // iterate over positive numbers in ascending numbers
    if (!bucket_found) {
        for (bucket_index = 0; bucket_index < buckets.size() / 2; bucket_index++) {
            item_count += buckets[bucket_index];
            if (percentile_position < item_count) {
                bucket_found = true;
                break;
            }
        }
    }

    uint64_t percentile_pos_in_bucket = percentile_position - (item_count - buckets[bucket_index]);

    return std::pair(percentile_pos_in_bucket, bucket_index);
}

std::pair<uint64_t , uint64_t > find_percentile_position_in_subbucket(uint64_t percentile_pos_in_subbucket, std::vector<uint64_t > buckets, uint64_t base_bucket) {
    uint64_t percentile_position = percentile_pos_in_subbucket;

    uint64_t  item_count = 0;

    // printf("percentile_pos=%lu\n", percentile_position);
    // printf("buckets_total_items=%lu\n", buckets_total_items);
    uint64_t bucket_index = buckets.size() - 1;
    bool bucket_found = false;
    if (base_bucket >= BUCKET_COUNT / 2) {
        for (bucket_index = buckets.size() - 1; bucket_index >= 0; bucket_index--) {
            item_count += buckets[bucket_index];
            if (percentile_position < item_count) {
                break;
            }
        }
    } else {
        for (bucket_index = 0; bucket_index < buckets.size(); bucket_index++) {
            item_count += buckets[bucket_index];
            if (percentile_position < item_count) {
                break;
            }
        }
    }

    uint64_t percentile_pos_in_bucket = percentile_position - (item_count - buckets[bucket_index]);

    return std::pair(percentile_pos_in_bucket, bucket_index);
}

std::pair<double, std::pair<uint64_t, uint64_t>>
find_percentile_in_histogram(uint64_t percentile_position_in_bucket, std::map<double, bucket_item *> &numbers_in_bucket) {
    auto result_key = numbers_in_bucket.begin()->first;
    auto result_item = numbers_in_bucket.begin()->second;
    uint64_t sum = 0;

    for (auto const&[key, val] : numbers_in_bucket) {
        sum += val->count;
        if (sum > percentile_position_in_bucket) {
            result_key = key;
            result_item = val;
            break;
        }
    }

    return std::pair(result_key, std::pair(result_item->lowest_index * 8, result_item->highest_index * 8));
}
