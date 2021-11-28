//
// Created by topnax on 28.11.21.
//
#include <iostream>
#include <fstream>
#include "params.h"
#include "opencl_utils.h"

bool parse_params_from_args(int argc, char *argv[], parameters &parameters) {
    if (argc != 4) {
        return false;
    }

    // parse file name and check whether the file can be read
    std::string file_name = argv[1];
    std::ifstream file(file_name, std::ios::binary);

    bool readable = file.good();
    file.close();
    if (!readable) {
        std::wcout << "File cannot be read." << std::endl << std::endl;
        // file cannot be read
        return false;
    }
    parameters.file_name = file_name;

    // parse percentile number and check constraints
    int percentile = -1;
    try {
        percentile = std::stoi(argv[2]);
    }
    catch (const std::exception &e) {}
    parameters.percentile = percentile;

    // percentile percentile constraints
    if (percentile < 1 || percentile > 100) {
        std::wcout << "Percentile must be an integer from the range <1, 100>." << std::endl << std::endl;
        return false;
    }

    // parse the processor
    std::string processor = argv[3];
    if (processor == PROCESSOR_NAME_SINGLE) {
        parameters.mode = SERIAL;
    } else if (processor == PROCESSOR_NAME_SMP) {
        parameters.mode = SMP;
    } else {
        parameters.mode = OPENCL;
        cl::Device device;
        // find whether device with the given name exists
        bool device_found = cl_get_device(processor, device);
        if (!device_found) {
            std::wcout << "OpenCL device \"" << processor.c_str() << "\" not available." << std::endl << std::endl;
            return false;
        }
        parameters.device = device;
    }

    return true;
}

void print_help() {
    std::wcout << "Invalid program arguments, correct program usage is:" << std::endl << std::endl
               << "$ " << PROGRAM_NAME << " file_name percentile processor" << std::endl << std::endl
               << "file_name   - name of the file to be processed" << std::endl
               << "percentile  - integer specifying the percentile to be found <1-100>" << std::endl
               << R"(processor   - "single" (single-threaded processing), "smp" (multi-threaded processing) or name of an OpenCL device to be used for processing)"
               << std::endl;
}
