//
// Created by topnax on 10.10.21.
//

#include "bucketing.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <bitset>

const unsigned long DOUBLE_FLOAT_EXPONENT_MASK = 0x7FF0000000000000;
const unsigned long DOUBLE_FLOAT_MANTISSA_MASK = 0xFFFFFFFFFFFFF;

struct bucket_item {
    long count;
    long lowest_index;
    long highest_index;
};

std::pair<std::vector<long>, long> create_buckets(char *file_name) {
    std::vector<long> buckets(BUCKET_COUNT) ;
    long buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer (NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    while (true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        for (int i = 0; i < numbers_read; ++i) {
            unsigned long content = *((unsigned long*) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

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

std::pair<std::vector<long>, long> create_sub_buckets(char *file_name, unsigned long base_bucket_index) {
    std::vector<long> sub_buckets(SUB_BUCKET_COUNT) ;
    long buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer (NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    while (true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        for (int i = 0; i < numbers_read; ++i) {
            unsigned long content = *((unsigned long*) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

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

            if (bucket_index != base_bucket_index) {
                continue;
            }

            unsigned long sub_bucket_index = (content & SUB_BUCKET_MANTISSA_MASK) >> SUB_BUCKET_SHIFT;

            //printf("%ld offset\n", SUB_BUCKET_SHIFT);
            //std::cout << "c = " << std::bitset<64>(sub_bucket_index)  << std::endl;

            sub_buckets[sub_bucket_index]++;
            buckets_total_items++;
        }
    }

    return std::pair(sub_buckets, buckets_total_items);
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

std::pair<long, long> find_percentile_position_in_subbucket(unsigned long percentile_pos_in_subbucket, std::vector<long> buckets, unsigned long buckets_total_items, unsigned long base_bucket) {
    unsigned long percentile_position = percentile_pos_in_subbucket;

    long item_count = 0;

    // printf("percentile_pos=%lu\n", percentile_position);
    // printf("buckets_total_items=%lu\n", buckets_total_items);
    unsigned long bucket_index = buckets.size() - 1;
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

    unsigned long percentile_pos_in_bucket = percentile_position - (item_count - buckets[bucket_index]);

    return std::pair(percentile_pos_in_bucket, bucket_index);
}

std::pair<double, std::pair<long, long>> find_percentile_value(long bucket, long percentile_position_in_bucket, char *file_name) {
    std::map<double, bucket_item*> numbers_in_bucket;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer (NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    long index = 0;

    while(true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        for (int i = 0; i < numbers_read; ++i) {
            unsigned long content = *((unsigned long *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

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
                double number = *((double *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);
                auto pos = numbers_in_bucket.find(number);
                if (pos == numbers_in_bucket.end()) {
                    bucket_item *item = new bucket_item; // TODO delete
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

std::pair<double, std::pair<long, long>> find_percentile_value_subbucket(long bucket, long percentile_position_in_bucket, char *file_name, long subbucket) {
    std::map<double, bucket_item*> numbers_in_bucket;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer (NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    long index = 0;

    while(true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        for (int i = 0; i < numbers_read; ++i) {
            unsigned long content = *((unsigned long *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

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
                unsigned long sub_bucket_index = (content & SUB_BUCKET_MANTISSA_MASK) >> SUB_BUCKET_SHIFT;

                if (sub_bucket_index == subbucket) {
                    double number = *((double *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);
                    auto pos = numbers_in_bucket.find(number);
                    if (pos == numbers_in_bucket.end()) {
                        bucket_item *item = new bucket_item; // TODO delete
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
            }

            index++;
        }
    }

    long sum = 0;
    auto result_key = numbers_in_bucket.begin()->first;
    auto result_item = numbers_in_bucket.begin()->second;
    for (auto const& [key, val] : numbers_in_bucket)
    {
        auto result = std::pair(result_key, std::pair(result_item->lowest_index, result_item->highest_index));
    }

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
