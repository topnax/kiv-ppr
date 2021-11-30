//
// Created by topnax on 20.11.21.
//

#include "../solution.h"
#include "smp_solution.h"
#include "../bucketing/bucketing_utils.h"
#include "smp_bucketing.h"


solution_result process_file_smp(char *file_name, int percentile) {
    // create buckets
    auto buckets_and_item_count = create_buckets_smp(file_name);

    // find the percentile position
    auto position = find_percentile_position(percentile, buckets_and_item_count.first, buckets_and_item_count.second);

    // find the value of the percentile
    auto result = find_percentile_value_smp(
            position.second,
            position.first, file_name
    );

    return result;
}
