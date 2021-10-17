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

    auto result = find_percentile_value(percentile_pos_in_bucket_and_bucket_index.second, percentile_pos_in_bucket_and_bucket_index.first, file_name);

    std::wcout << result.first << " " << result.second.first << " " << result.second.second << std::endl;
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
