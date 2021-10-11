//
// Created by topnax on 10.10.21.
//

#include "bucketing.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <map>

std::pair<std::vector<long>, long> create_buckets(char *file_name) {
    std::vector<long> buckets(BUCKET_COUNT) ;
    long buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a 8 byte buffer (for 64-bit float)
    std::vector<char> buffer (NUMBER_SIZE, 0);

    bool eof = false;

    while(!eof) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        // TODO ignore numbers with exponents all 0s and all 1s

        // interpret the buffer as a long
        unsigned long bucket_index = (*((unsigned long*) &buffer[0])) >> NUMBER_SHIFT;

        buckets[bucket_index]++;
        buckets_total_items++;

        if (fin.gcount() < NUMBER_SIZE) eof = true;
    }

    return std::pair(buckets, buckets_total_items);
}

std::pair<long, long> find_percentile_position(int percentile, std::vector<long> buckets, long buckets_total_items) {
    long percentile_position = buckets_total_items * ((double) percentile / (double) 100);

    long item_count = 0;
    long bucket_index = 0;

    printf("percentile_pos: %ld, buckets_total_items: %ld\n", percentile_position, buckets_total_items);

    do {
       item_count += buckets[bucket_index];
    } while (percentile_position > item_count && ++bucket_index < buckets.size());

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

    // prepare a 8 byte buffer (for 64-bit float)
    std::vector<char> buffer (NUMBER_SIZE, 0);

    long index = 0;
    bool eof = false;

    while(!eof) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());
        std::streamsize s=fin.gcount();

        // TODO ignore numbers with exponents all 0s and all 1s

        // interpret the buffer as a long
        unsigned long bucket_index = (*((unsigned long*) &buffer[0])) >> NUMBER_SHIFT;

        if (bucket_index == bucket) {
            double number = *((double*) &buffer[0]) ;
            auto pos = numbers_in_bucket.find(number);
            if (pos == numbers_in_bucket.end()) {
                bucket_item* item = new bucket_item;
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

        if (fin.gcount() < NUMBER_SIZE) eof = true;

        index++;
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


