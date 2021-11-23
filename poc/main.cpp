// Your First C++ Program (kappa)

#include <iostream>
#include <vector>
#include "bucketing.h"
#include "smp_solution.h"
#include "bucketing_constants.h"
#include "bucketing_utils.h"

// TODO inspect this:
#include<bits/stdc++.h> //Headerfile which include all other headerfiles in c++


solution_result process_file(char *file_name, int percentile) {
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

        return result_sub;
    } else {
        auto result = find_percentile_value(
                percentile_pos_in_bucket_and_bucket_index.second,
                percentile_pos_in_bucket_and_bucket_index.first, file_name
        );

        return result;
    }
}


int main(int argc, char *argv[]) {
    // TODO arg parsing
    int percentile = atoi(argv[2]);

    solution_result result;
    auto mode = std::string(argv[3]);
    if (mode == "smp") {
        result = process_file_smp(argv[1], percentile);
    } else if (mode == "serial") {
        result = process_file(argv[1], percentile);
    } else {
        std::wcout << "Invalid mode supplied." << std::endl;
        return -1;
    }

    print_solution_result(result);

    return 0;
}
