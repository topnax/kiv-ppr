//
// Created by topnax on 10.10.21.
//

#include "../bucketing/bucketing_constants.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <limits>

std::vector<double> load_doubles(char *file_name, int floats_per_read) {
    size_t buffer_size = floats_per_read * sizeof (double);

    std::vector<double> doubles;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);
    bool eof = false;

    // prepare a 8 byte buffer (for 64-bit float)
    std::vector<char> buffer(buffer_size, 0);

    while (!eof) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        // interpret the buffer as a double
        auto *read_doubles = (double *) buffer.data();

        for (int i = 0; i < fin.gcount() / sizeof(double); i++) {
            double d = read_doubles[i];
            doubles.push_back(d);
        }

        if (fin.gcount() < NUMBER_SIZE_BYTES) eof = true;
    }

    return doubles;
}

double find_percentile(int percentile, std::vector<double> &doubles) {
    return doubles[(doubles.size() - 1) * ((double) percentile / (double) 100)];
}

std::vector<std::pair<long, long>> find_indices(std::vector<double> &values, std::vector<double> &doubles) {
    std::vector<std::pair<long, long>> result(values.size());

    for (auto &i : result) {
        i.first = std::numeric_limits<long>::max();
        i.second = 0;
    }

    for (long index = 0; index < doubles.size(); index++) {
        double d = doubles[index];
        for (int i = 0; i < values.size(); i++) {
            auto v = values[i];
            if (d == v) {
                if (index * 8 < result[i].first) {
                    result[i].first = index * 8;
                }
                if (index * 8 > result[i].second) {
                    result[i].second = index * 8;
                }
            }
        }
    }
    return result;
}

int main(int argc, char *argv[]) {
    int floats_per_read = atoi(argv[2]);
    auto doubles = load_doubles(argv[1], floats_per_read);
    auto filtered_doubles = std::vector<double>();
    // filter doubles that are not a valid double
    std::copy_if(doubles.begin(), doubles.end(), std::back_inserter(filtered_doubles),
                 [](const double d)
                 {
                     auto cls = std::fpclassify(d);
                     if (cls == FP_NORMAL || cls == FP_ZERO) {
                         return true;
                     }
                     return false;
                 });
    // sort filtered doubles
    std::sort(filtered_doubles.begin(), filtered_doubles.end());

    std::vector<int> percentiles = {10, 20, 30, 40, 50, 60, 70, 80, 90};

    std::vector<double> percentile_values(percentiles.size());
    std::vector<std::pair<long, long>> percentile_indices(percentiles.size());

    for (int i = 0; i < percentiles.size(); i++) {
        percentile_values[i] = find_percentile(percentiles[i], filtered_doubles);
    }

    auto indices = find_indices(percentile_values, doubles);

    for (int i = 0; i < percentiles.size(); i++) {
        auto result = indices[i];
        std::wcout << std::hexfloat << percentiles[i] << ". " << percentile_values[i] << " " << result.first << " " << result.second << std::endl;
    }

    return 0;
}
