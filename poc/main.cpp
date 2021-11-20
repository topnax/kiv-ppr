// Your First C++ Program (kappa)

#include <iostream>
#include <vector>
#include "bucketing.h"
#include "smp_solution.h"
#include "bucketing_constants.h"
#include "bucketing_utils.h"

// TODO inspect this:
#include<bits/stdc++.h> //Headerfile which include all other headerfiles in c++


void process_file(char *file_name, int percentile) {
    // TODO move to single_threaded_solution
    auto buckets = create_buckets(file_name);

    auto percentile_pos_in_bucket_and_bucket_index = find_percentile_position(percentile, buckets.first,
                                                                              buckets.second);

    if (buckets.first[percentile_pos_in_bucket_and_bucket_index.second] > MAX_BUCKET_COUNT) {
        // perform subbucketing
        auto sub_buckets = create_sub_buckets(file_name, percentile_pos_in_bucket_and_bucket_index.second);

        auto percentile_pos_in_subbucket_and_subbucket_index = find_percentile_position_in_subbucket(
                percentile_pos_in_bucket_and_bucket_index.first,
                sub_buckets.first,
                percentile_pos_in_bucket_and_bucket_index.second
        );


        auto result_sub = find_percentile_value_subbucket(
                percentile_pos_in_bucket_and_bucket_index.second,
                percentile_pos_in_subbucket_and_subbucket_index.first,
                file_name,
                percentile_pos_in_subbucket_and_subbucket_index.second
        );

        std::wcout << result_sub.first << " " << result_sub.second.first << " " << result_sub.second.second
                   << std::endl;
    } else {
        auto result = find_percentile_value(
                percentile_pos_in_bucket_and_bucket_index.second,
                percentile_pos_in_bucket_and_bucket_index.first, file_name
        );
        std::wcout << result.first << " " << result.second.first << " " << result.second.second << std::endl;
    }
}


int main(int argc, char *argv[]) {
    // TODO arg parsing

    //process_file(argv[1], 50);
    int percentile = atoi(argv[2]);

    process_file(argv[1], percentile);
    process_file_smp(argv[1], percentile);

    return 0;
}
