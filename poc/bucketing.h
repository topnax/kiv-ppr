//
// Created by topnax on 10.10.21.
//

#include <vector>
#include <math.h>

#ifndef POC_BUCKETING_H
#define POC_BUCKETING_H

const int NUMBER_BUFFER_SIZE = 1000;
const int NUMBER_SIZE_BYTES = 8;
const int NUMBER_SIZE_BITS = NUMBER_SIZE_BYTES * 8;

const unsigned long NUMBER_MAX = 0xFFFFFFFFFFFFFFFFu;

const int BUCKET_BITS = 22;
const int NUMBER_SHIFT = NUMBER_SIZE_BITS - BUCKET_BITS;

const int BUCKET_KEY_LENGTH_BITS = NUMBER_SIZE_BITS - NUMBER_SHIFT;
const unsigned long BUCKET_COUNT = pow(2, BUCKET_KEY_LENGTH_BITS);

const unsigned long SUB_BUCKET_MANTISSA_BITS = 20;
const unsigned long SUB_BUCKET_COUNT = pow(2, SUB_BUCKET_MANTISSA_BITS);
const unsigned long SUB_BUCKET_MANTISSA_MASK =
        ((NUMBER_MAX >> (NUMBER_SIZE_BITS - (BUCKET_BITS + SUB_BUCKET_MANTISSA_BITS)))
                << (NUMBER_SIZE_BITS - SUB_BUCKET_MANTISSA_BITS)) >> BUCKET_BITS;
const unsigned long SUB_BUCKET_SHIFT = NUMBER_SIZE_BITS - (SUB_BUCKET_MANTISSA_BITS + BUCKET_BITS);

const unsigned long SIGN_BIT_MASK = 0x8000000000000000;

const unsigned long MEBI_BYTE = pow(pow(2, 10), 2);
const unsigned long MEMORY_LIMIT = MEBI_BYTE * 200;
const unsigned long MAX_BUCKET_COUNT = (unsigned long) ((double) MEMORY_LIMIT * 0.8);

std::pair<std::vector<long>, long> create_buckets(char *file_name);

std::pair<std::vector<long>, long> create_sub_buckets(char *file_name, unsigned long base_bucket_index);

std::pair<long, long> find_percentile_position(int percentile, std::vector<long> buckets, long buckets_total_items);

std::pair<long, long>
find_percentile_position_in_subbucket(unsigned long percentile_pos_in_subbucket, std::vector<long> buckets,
                                      unsigned long buckets_total_items, unsigned long base_bucket);

std::pair<double, std::pair<long, long>>
find_percentile_value(long bucket, long percentile_position_in_bucket, char *file_name);

std::pair<double, std::pair<long, long>>
find_percentile_value_subbucket(long bucket, long percentile_position_in_bucket, char *file_name, long subbucket);

#endif //POC_BUCKETING_H
