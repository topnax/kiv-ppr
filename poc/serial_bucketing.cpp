//
// Created by topnax on 10.10.21.
//

#include "serial_bucketing.h"
#include "bucketing_utils.h"
#include "bucketing_constants.h"
#include "double_utils.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <bitset>
#include <memory>
#include "watchdog.h"


std::pair<std::vector<uint64_t>, uint64_t> create_buckets_serial(char *file_name) {
    std::vector<uint64_t> buckets(BUCKET_COUNT);
    uint64_t buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer(NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    while (true) {
        ThreadWatchdog::kick();
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

            buckets[bucket_index]++;
            buckets_total_items++;
        }
    }

    return std::pair(buckets, buckets_total_items);
}

std::pair<std::vector<uint64_t>, uint64_t> create_sub_buckets_serial(char *file_name, uint64_t base_bucket_index) {
    std::vector<uint64_t> sub_buckets(SUB_BUCKET_COUNT);
    uint64_t buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer(NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    while (true) {
        ThreadWatchdog::kick();
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

            if (bucket_index != base_bucket_index) {
                continue;
            }

            uint64_t sub_bucket_index = (content & SUB_BUCKET_MANTISSA_MASK) >> SUB_BUCKET_SHIFT;

            sub_buckets[sub_bucket_index]++;
            buckets_total_items++;
        }
    }

    return std::pair(sub_buckets, buckets_total_items);
}

std::pair<double, std::pair<uint64_t, uint64_t>>
find_percentile_value_serial(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name) {
    std::map<double, std::unique_ptr<bucket_item>> numbers_in_bucket;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer(NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    uint64_t index = 0;

    while (true) {
        ThreadWatchdog::kick();
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        for (int i = 0; i < numbers_read; ++i) {
            uint64_t content = *((uint64_t *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

            if (is_valid_double(content)) {
                // interpret the buffer as a long
                uint64_t bucket_index = content >> NUMBER_SHIFT;

                if (bucket_index == bucket) {
                    update_histogram_item(content, numbers_in_bucket, index);
                }
            }
            index++;
        }
    }

    return find_percentile_in_histogram(percentile_position_in_bucket, numbers_in_bucket);
}

std::pair<double, std::pair<uint64_t, uint64_t>>
find_percentile_value_subbucket_serial(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name,
                                uint64_t subbucket) {
    std::map<double, std::unique_ptr<bucket_item>> numbers_in_bucket;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer(NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    uint64_t index = 0;

    while (true) {
        ThreadWatchdog::kick();
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        for (int i = 0; i < numbers_read; ++i) {
            uint64_t content = *((uint64_t *) &buffer[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

            if (is_valid_double(content)) {
                // interpret the buffer as a long
                uint64_t bucket_index = content >> NUMBER_SHIFT;

                if (bucket_index == bucket) {
                    uint64_t sub_bucket_index = (content & SUB_BUCKET_MANTISSA_MASK) >> SUB_BUCKET_SHIFT;

                    if (sub_bucket_index == subbucket) {
                        update_histogram_item(content, numbers_in_bucket, index);
                    }
                }
            }
            index++;
        }
    }

    return find_percentile_in_histogram(percentile_position_in_bucket, numbers_in_bucket);
}
