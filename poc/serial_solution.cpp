//
// Created by topnax on 23.11.21.
//

#include "solution.h"
#include "serial_bucketing.h"
#include "bucketing_constants.h"

solution_result process_file_serial(char *file_name, int percentile) {
    auto buckets = create_buckets_serial(file_name);

    auto percentile_pos_in_bucket_and_bucket_index = find_percentile_position(percentile, buckets.first,
                                                                              buckets.second);

    if (buckets.first[percentile_pos_in_bucket_and_bucket_index.second] > MAX_BUCKET_COUNT) {
        // perform subbucketing
        auto sub_buckets = create_sub_buckets_serial(file_name, percentile_pos_in_bucket_and_bucket_index.second);

        auto percentile_pos_in_subbucket_and_subbucket_index = find_percentile_position_in_subbucket(
                percentile_pos_in_bucket_and_bucket_index.first,
                sub_buckets.first,
                percentile_pos_in_bucket_and_bucket_index.second
        );


        auto result_sub = find_percentile_value_subbucket_serial(
                percentile_pos_in_bucket_and_bucket_index.second,
                percentile_pos_in_subbucket_and_subbucket_index.first,
                file_name,
                percentile_pos_in_subbucket_and_subbucket_index.second
        );

        return result_sub;
    } else {
        auto result = find_percentile_value_serial(
                percentile_pos_in_bucket_and_bucket_index.second,
                percentile_pos_in_bucket_and_bucket_index.first, file_name
        );

        return result;
    }
}
