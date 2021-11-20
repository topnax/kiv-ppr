//
// Created by topnax on 20.11.21.
//

#include <iostream>
#include <tbb/concurrent_vector.h>
#include "smp_solution.h"
#include "bucketing.h"
#include "smp_bucketing.h"


void process_file_smp(char *file_name, int percentile) {
    auto buckets_and_item_count = create_buckets_smp(file_name);
    std::wcout << "bucket item count: " << buckets_and_item_count.second << std::endl;
    // TODO calculate percentile position

    // TODO find the actual percentile value
}
