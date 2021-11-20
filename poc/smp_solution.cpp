//
// Created by topnax on 20.11.21.
//

#include <iostream>
#include <tbb/concurrent_vector.h>
#include "smp_solution.h"
#include "bucketing_utils.h"
#include "smp_bucketing.h"
#include <chrono>


void process_file_smp(char *file_name, int percentile) {
    auto buckets_and_item_count = create_buckets_smp(file_name);

    auto position = find_percentile_position(percentile, buckets_and_item_count.first, buckets_and_item_count.second);

    // TODO calculate percentile value
}
