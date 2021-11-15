//
// Created by topnax on 10.10.21.
//

#include "bucketing.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>

const unsigned long DOUBLE_FLOAT_EXPONENT_MASK = 0x7FF0000000000000;
const unsigned long DOUBLE_FLOAT_MANTISSA_MASK = 0xFFFFFFFFFFFFF;


std::pair<std::vector<long>, long> create_buckets(char *file_name) {
    std::vector<long> buckets(BUCKET_COUNT) ;
    long buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer (NUMBER_SIZE * NUMBER_BUFFER_SIZE, 0);

    while (true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE;

        if (numbers_read < 1) break;

        for (int i = 0; i < numbers_read; ++i) {
            unsigned long content = *((unsigned long*) &buffer[i * (NUMBER_SIZE / sizeof(char))]);

            // filter out SUBNORMAL, INFINITE or NAN numbers
            auto exponent = content & DOUBLE_FLOAT_EXPONENT_MASK;
            if (exponent == 0) {
                if ((content & DOUBLE_FLOAT_MANTISSA_MASK) != 0)  {
                    // SUBNORMAL
                    continue;
                }
            } else if (exponent == DOUBLE_FLOAT_EXPONENT_MASK) {
                // INFINITE / NAN
                continue;
            }

            // interpret the buffer as a long
            unsigned long bucket_index = content >> NUMBER_SHIFT;

            buckets[bucket_index]++;
            buckets_total_items++;
        }
    }

    return std::pair(buckets, buckets_total_items);
}

std::pair<long, long> find_percentile_position(int percentile, std::vector<long> buckets, long buckets_total_items) {
    long percentile_position = buckets_total_items * ((double) percentile / (double) 100);

    long item_count = 0;


    unsigned long bucket_index = buckets.size() - 1;
    bool bucket_found = false;
    // iterate over negative numbers in ascending order
    for (; bucket_index > buckets.size() / 2; bucket_index--) {
        item_count += buckets[bucket_index];
        if (percentile_position < item_count) {
            bucket_found = true;
            break;
        }
    }

    // iterate over positive numbers in ascending numbers
    if (!bucket_found)  {
        for (bucket_index = 0; bucket_index <= buckets.size() / 2; bucket_index++) {
            item_count += buckets[bucket_index];
            if (percentile_position < item_count) {
                bucket_found = true;
                break;
            }
        }
    }

    long percentile_pos_in_bucket = percentile_position - (item_count - buckets[bucket_index]);

    return std::pair(percentile_pos_in_bucket, bucket_index);
}

struct bucket_item {
    long count;
    long lowest_index;
    long highest_index;
};

std::pair<double, std::pair<long, long>> find_percentile_value(long bucket, long percentile_position_in_bucket, char *file_name) {
    std::map<double, bucket_item*> numbers_in_bucket;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer (NUMBER_SIZE * NUMBER_BUFFER_SIZE, 0);

    long index = 0;

    while(true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE;

        if (numbers_read < 1) break;

        for (int i = 0; i < numbers_read; ++i) {
            unsigned long content = *((unsigned long *) &buffer[i * (NUMBER_SIZE / sizeof(char))]);

            // filter out SUBNORMAL, INFINITE or NAN numbers
            auto exponent = content & DOUBLE_FLOAT_EXPONENT_MASK;
            if (exponent == 0) {
                if ((content & DOUBLE_FLOAT_MANTISSA_MASK) != 0) {
                    // SUBNORMAL
                    continue;
                }
            } else if (exponent == DOUBLE_FLOAT_EXPONENT_MASK) {
                // INFINITE / NAN
                continue;
            }

            // interpret the buffer as a long
            unsigned long bucket_index = content >> NUMBER_SHIFT;

            if (bucket_index == bucket) {
                double number = *((double *) &buffer[i * (NUMBER_SIZE / sizeof(char))]);
                auto pos = numbers_in_bucket.find(number);
                if (pos == numbers_in_bucket.end()) {
                    bucket_item *item = new bucket_item;
                    item->count = 1;
                    item->lowest_index = index;
                    item->highest_index = index;
                    numbers_in_bucket[number] = item;
                } else {
                    auto item = pos->second;
                    item->count = item->count + 1;
                    if (item->lowest_index > index) {
                        item->lowest_index = index;
                    }
                    if (item->highest_index < index) {
                        item->highest_index = index;
                    }
                }
            }

            index++;
        }
    }

    long sum = 0;
    auto result_key = numbers_in_bucket.begin()->first;
    auto result_item = numbers_in_bucket.begin()->second;
    for (auto const& [key, val] : numbers_in_bucket)
    {
        if (sum >= percentile_position_in_bucket) {
            result_key = key;
            result_item = val;
            break;
        }
        sum += val->count;
    }

    return std::pair(result_key, std::pair(result_item->lowest_index, result_item->highest_index));
}
