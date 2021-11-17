// Your First C++ Program

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "bucketing.h"



#include<bits/stdc++.h> //Headerfile which include all other headerfiles in c++


void process_file(char* file_name, int percentile) {
    auto buckets = create_buckets(file_name);

    auto percentile_pos_in_bucket_and_bucket_index = find_percentile_position(percentile, buckets.first, buckets.second);

    //printf("bucket %ld has %ld items\n", percentile_pos_in_bucket_and_bucket_index.second, buckets.first[percentile_pos_in_bucket_and_bucket_index.second]);
    if (buckets.first[percentile_pos_in_bucket_and_bucket_index.second] > MAX_BUCKET_COUNT) {
        // perform subbucketing
        auto sub_buckets = create_sub_buckets(file_name, percentile_pos_in_bucket_and_bucket_index.second);

        auto percentile_pos_in_subbucket_and_subbucket_index = find_percentile_position_in_subbucket(
                percentile_pos_in_bucket_and_bucket_index.first,
                sub_buckets.first,
                sub_buckets.second,
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

void print_binary(std::vector<char> vc) {
    for (int i = 0; i < vc.size(); i++) {
        char c = vc[i];
        for (int j = 0; j < 8; j++) {
            std::cout << ((c & (int) (pow(2, 7 - j))) ? "1" : "0");
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {

    //process_file(argv[1], 50);
    int percentile = atoi(argv[2]);
    process_file(argv[1], percentile);

    return 0;
}
