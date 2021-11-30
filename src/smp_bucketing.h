//
// Created by topnax on 20.11.21.
//
#pragma once

#include <vector>
#include <cstdint>

// alias for the pair used
using buckets_and_item_count = std::pair<std::vector<uint64_t>, uint64_t>;

/**
 * Create buckets based on the file's content using multiple threads
 *
 * @param file_name name of the file to be processed
 *
 * @return array representing the bucket histogram and the number of items present in buckets
 */
buckets_and_item_count create_buckets_smp(char *file_name);

/**
 * Find percentile value in the given histogram using the bucket it is present in using multiple threads
 *
 * @param bucket the bucket the percentile lies in
 * @param percentile_position_in_bucket position of the percentile relative to the bucket it belongs to
 * @param file_name name of the file to be processed
 *
 * @return the percentile value, its first and last occurrence
 */
std::pair<double, std::pair<uint64_t, uint64_t>> find_percentile_value_smp(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name);
