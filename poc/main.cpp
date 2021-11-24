// Your First C++ Program (kappa)

#include <iostream>
#include "bucketing_constants.h"
#include "bucketing_utils.h"
#include "solution.h"

// TODO inspect this:
#include<bits/stdc++.h> //Headerfile which include all other headerfiles in c++



int main(int argc, char *argv[]) {
    // TODO arg parsing
    int percentile = atoi(argv[2]);

    solution_result result;
    auto mode_raw = std::string(argv[3]);
    auto mode = NONE;

    if (mode_raw == "serial")  {
        mode = SERIAL;
    } else if (mode_raw == "smp") {
        mode = SMP;
    } else if (mode_raw == "opencl") {
        mode = OPENCL;
    }

    result = process_for_solution(argv[1], percentile, mode);

    print_solution_result(result);

    return 0;
}
