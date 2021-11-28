#include "solution.h"
#include "params.h"


int main(int argc, char *argv[]) {
    auto params = parameters{};

    if (!parse_params_from_args(argc, argv, params)) {
        print_help();
    } else {
        auto result = process_for_solution(params);
        print_solution_result(result);
    }

    return 0;
}
