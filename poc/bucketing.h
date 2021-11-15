//
// Created by topnax on 10.10.21.
//

#include <vector>
#include <math.h>

#ifndef POC_BUCKETING_H
#define POC_BUCKETING_H

const int NUMBER_BUFFER_SIZE = 1000;
const int NUMBER_SIZE = 8;
const int NUMBER_SHIFT = 44;
const int BUCKET_KEY_LENGTH_BITS = (NUMBER_SIZE * 8) - NUMBER_SHIFT;
const long BUCKET_COUNT = pow(2, BUCKET_KEY_LENGTH_BITS);
const unsigned long SIGN_BIT_MASK = 0x8000000000000000;

std::pair<std::vector<long>, long> create_buckets(char* file_name);

std::pair<long, long> find_percentile_position(int percentile, std::vector<long> buckets, long buckets_total_items);

std::pair<double, std::pair<long, long>> find_percentile_value(long bucket, long percentile_position_in_bucket, char *file_name);

#endif //POC_BUCKETING_H
