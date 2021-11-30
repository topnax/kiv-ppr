//
// Created by topnax on 20.11.21.
//

#include <iostream>
#include <ios>
#include <memory>
#include "bucketing_utils.h"
#include "bucketing_constants.h"

std::pair<uint64_t , uint64_t > find_percentile_position(int percentile, std::vector<uint64_t > buckets, uint64_t  buckets_total_items) {
    uint64_t percentile_position = static_cast<uint64_t>((buckets_total_items - 1) * ((double) percentile / (double) 100));

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

    size_t bucket_index = buckets.size() - 1;
    bool bucket_found = false;
    if (base_bucket >= BUCKET_COUNT / 2) {
        for (bucket_index = buckets.size() - 1; ; bucket_index--) {
            item_count += buckets[bucket_index];
            if (percentile_position < item_count || bucket_index <= 0) {
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
find_percentile_in_histogram(uint64_t percentile_position_in_bucket, std::map<double, std::unique_ptr<bucket_item>> &numbers_in_bucket) {
    auto result_key = numbers_in_bucket.begin()->first;
    auto result_item = numbers_in_bucket.begin()->second.get();
    uint64_t sum = 0;

    for (auto const&[key, val] : numbers_in_bucket) {
        sum += val->count;
        if (sum > percentile_position_in_bucket) {
            result_key = key;
            result_item = val.get();
            break;
        }
    }

    return std::pair(result_key, std::pair(result_item->lowest_index * 8, result_item->highest_index * 8));
}

// inlining this causes error during compilation
void update_histogram_item(uint64_t &content, std::map<double, std::unique_ptr<bucket_item>> &numbers_in_bucket, uint64_t index) {
    auto number = *((double *) &content);
    auto pos = numbers_in_bucket.find(number);
    if (pos == numbers_in_bucket.end()) {
        auto item = new bucket_item;
        item->count = 1;
        item->lowest_index = index;
        item->highest_index = index;
        numbers_in_bucket[number] = std::unique_ptr<bucket_item>(item);
    } else {
        auto item = pos->second.get();
        item->count = item->count + 1;
        if (item->lowest_index > index) {
            item->lowest_index = index;
        }
        if (item->highest_index < index) {
            item->highest_index = index;
        }
    }
}
