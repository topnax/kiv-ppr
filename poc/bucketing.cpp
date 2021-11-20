//
// Created by topnax on 10.10.21.
//

#include "bucketing.h"
#include "bucketing_constants.h"
#include "double_utils.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <bitset>


struct bucket_item {
    long count;
    long lowest_index;
    long highest_index;
};

std::pair<std::vector<uint64_t>, uint64_t> create_buckets(char *file_name) {
    std::vector<uint64_t> buckets(BUCKET_COUNT) ;
    uint64_t buckets_total_items = 0;

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
            uint64_t content = *((uint64_t*) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

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
            uint64_t bucket_index = content >> NUMBER_SHIFT;

            buckets[bucket_index]++;
            buckets_total_items++;
        }
    }

    return std::pair(buckets, buckets_total_items);
}

std::pair<std::vector<uint64_t>, uint64_t> create_sub_buckets(char *file_name, uint64_t base_bucket_index) {
    std::vector<uint64_t> sub_buckets(SUB_BUCKET_COUNT) ;
    uint64_t buckets_total_items = 0;

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
            uint64_t content = *((uint64_t *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

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
            uint64_t bucket_index = content >> NUMBER_SHIFT;

            if (bucket_index != base_bucket_index) {
                continue;
            }

            uint64_t sub_bucket_index = (content & SUB_BUCKET_MANTISSA_MASK) >> SUB_BUCKET_SHIFT;

            //printf("%ld offset\n", SUB_BUCKET_SHIFT);
            //std::cout << "c = " << std::bitset<64>(sub_bucket_index)  << std::endl;

            sub_buckets[sub_bucket_index]++;
            buckets_total_items++;
        }
    }

    return std::pair(sub_buckets, buckets_total_items);
}

std::pair<double, std::pair<uint64_t, uint64_t >> find_percentile_value(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name) {
    std::map<double, bucket_item*> numbers_in_bucket;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer (NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    uint64_t index = 0;

    while(true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        for (int i = 0; i < numbers_read; ++i) {
            uint64_t content = *((uint64_t *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

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
            uint64_t bucket_index = content >> NUMBER_SHIFT;

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

    uint64_t sum = 0;
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

std::pair<double, std::pair<uint64_t, uint64_t>> find_percentile_value_subbucket(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name, uint64_t subbucket) {
    std::map<double, bucket_item*> numbers_in_bucket;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer (NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    uint64_t index = 0;

    while(true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        for (int i = 0; i < numbers_read; ++i) {
            uint64_t content = *((uint64_t *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

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
            uint64_t bucket_index = content >> NUMBER_SHIFT;

            if (bucket_index == bucket) {
                uint64_t sub_bucket_index = (content & SUB_BUCKET_MANTISSA_MASK) >> SUB_BUCKET_SHIFT;

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

    uint64_t sum = 0;
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
