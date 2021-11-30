#include <iostream>
#include "solution.h"
#include "params.h"
#include "utils/watchdog.h"


int main(int argc, char *argv[]) {
    auto params = parameters{};

    if (!parse_params_from_args(argc, argv, params)) {
        // invalid parameters passed, print help
        print_help();
    } else {
        // create a new dog
        auto dog = Watchdog(WATCHDOG_DEFAULT_BITE_TIMEOUT, []() {
            std::wcout << "The thread watchdog has bitten, aborting the program." << std::endl;
            exit(-1);
        });
        // set the dog as a global dog
        ThreadWatchdog::set_doggo(&dog);
        // make it guard
        dog.guard();
        // kick it!
        dog.kick();

        // process the parameters
        auto result = process_for_solution(params);
        // print the results
        print_solution_result(result);

        // give the dog some rest
        dog.stop();
    }

    return 0;
}
