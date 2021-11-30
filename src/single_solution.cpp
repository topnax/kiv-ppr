//
// Created by topnax on 23.11.21.
//

#include "solution.h"
#include "single_bucketing.h"
#include "bucketing_constants.h"

solution_result process_file_single(char *file_name, int percentile) {
    // create buckets
    auto buckets = create_buckets_single(file_name);

    // find the position of the percentile
    auto percentile_pos_in_bucket_and_bucket_index = find_percentile_position(percentile, buckets.first,
                                                                              buckets.second);

    if (buckets.first[percentile_pos_in_bucket_and_bucket_index.second] > MAX_BUCKET_COUNT) {
        // perform sub bucketing
        auto sub_buckets = create_sub_buckets_single(file_name, percentile_pos_in_bucket_and_bucket_index.second);

        // find the position of the percentile in the sub bucket
        auto percentile_pos_in_subbucket_and_subbucket_index = find_percentile_position_in_subbucket(
                percentile_pos_in_bucket_and_bucket_index.first,
                sub_buckets.first,
                percentile_pos_in_bucket_and_bucket_index.second
        );

        // find the value of the percentile in a sub bucket
        auto result_sub = find_percentile_value_subbucket_single(
                percentile_pos_in_bucket_and_bucket_index.second,
                percentile_pos_in_subbucket_and_subbucket_index.first,
                file_name,
                percentile_pos_in_subbucket_and_subbucket_index.second
        );

        return result_sub;
    } else {
        // find the value of the percentile bucket
        auto result = find_percentile_value_single(
                percentile_pos_in_bucket_and_bucket_index.second,
                percentile_pos_in_bucket_and_bucket_index.first, file_name
        );

        return result;
    }
}
