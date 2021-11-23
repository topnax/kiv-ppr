//
// Created by topnax on 20.11.21.
//

#include "solution.h"
#include "smp_solution.h"
#include "bucketing_utils.h"
#include "smp_bucketing.h"


solution_result process_file_smp(char *file_name, int percentile) {
    auto buckets_and_item_count = create_buckets_smp(file_name);

    auto position = find_percentile_position(percentile, buckets_and_item_count.first, buckets_and_item_count.second);

    auto result = find_percentile_value_smp(
            position.second,
            position.first, file_name
    );

    return result;
}
