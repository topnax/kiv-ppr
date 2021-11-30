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
    // number of occurrences
    uint64_t count;
    // lowest index the number can be found at
    uint64_t lowest_index;
    // highest index the number can be found at
    uint64_t highest_index;
};

/**
 * Find the position of the percentile in the given vector of buckets based on the total number of items in buckets.
 *
 * @param percentile percentile to be found
 * @param buckets vector representing buckets (histogram using sub-intervals)
 * @param buckets_total_items total number of items present in buckets
 *
 * @return position of the percentile within the bucket it is in and the absolute position of the percentile relative
 * to the sorted list of all numbers
 */
std::pair<uint64_t, uint64_t> find_percentile_position(int percentile, std::vector<uint64_t> buckets, uint64_t buckets_total_items);

/**
 * Find the position of the percentile in the given sub bucket
 *
 * @param percentile_pos_in_subbucket percentile position in the sub bucket
 * @param buckets vector representing buckets (histogram using sub-intervals)
 * @param buckets_total_items total number of items present in buckets
 * @param base_bucket bucket the percentile to be found belongs to
 *
 * @return position of the percentile within the sub bucket it is in and the absolute position of the percentile relative
 * to the sorted list of all numbers.
 */
std::pair<uint64_t, uint64_t>
find_percentile_position_in_subbucket(uint64_t percentile_pos_in_subbucket, std::vector<uint64_t> buckets,
                                      uint64_t base_bucket);

/**
 * Find the percentile value (and its first and last occurrence) in the given histogram
 *
 * @param percentile_position_in_bucket position of the percentile within the bucket
 * @param numbers_in_bucket histogram in which the percentile should be found
 * @return the percentile value, its first and last occurrence
 */
std::pair<double, std::pair<uint64_t, uint64_t>>  find_percentile_in_histogram(uint64_t  percentile_position_in_bucket, std::map<double, std::unique_ptr<bucket_item>> &numbers_in_bucket);

/**
 * Updates the histogram item (increment occurrence count as well as first/last occurrence of the number)
 *
 * @param content number to be processed
 * @param numbers_in_bucket the histogram to be updated
 * @param index the index the number is found at
 */
void update_histogram_item(uint64_t &content, std::map<double, std::unique_ptr<bucket_item>> &numbers_in_bucket, uint64_t index);
