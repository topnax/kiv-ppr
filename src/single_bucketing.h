//
// Created by topnax on 10.10.21.
//
#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include "bucketing_utils.h"
#include "solution.h"

/**
 * Create buckets based on the file's content
 *
 * @param file_name name of the file to be processed
 *
 * @return array representing the bucket histogram and the number of items present in buckets
 */
std::pair<std::vector<uint64_t>, uint64_t> create_buckets_single(char *file_name);

/**
 * Create sub buckets based on the file's content for the given bucket
 *
 * @param file_name name of the file to be processed
 * @param base_bucket_index index of the base bucket
 *
 * @return array representing the sub bucket histogram and the number of items present in sub buckets
 */
std::pair<std::vector<uint64_t>, uint64_t> create_sub_buckets_single(char *file_name, uint64_t base_bucket_index);

/**
 * Find percentile value in the given histogram given it is present in the given bucket
 *
 * @param bucket the bucket the percentile lies within
 * @param percentile_position_in_bucket position of the percentile relative to the bucket it belongs to
 * @param file_name name of the file to be processed
 *
 * @return the percentile value, its first and last occurrence
 */
std::pair<double, std::pair<uint64_t, uint64_t>>
find_percentile_value_single(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name);

/**
 * The percentile
 *
 * @param bucket the bucket the percentile lies within
 * @param percentile_position_in_bucket position of the percentile relative to the bucket it belongs to
 * @param file_name name of the file to be processed
 * @param subbucket the sub bucket the percentile lies in
 *
 * @return the percentile value, its first and last occurrence
 */
solution_result
find_percentile_value_subbucket_single(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name, uint64_t subbucket);
