//
// Created by topnax on 28.11.21.
//
#pragma once

#include <memory>
#include "opencl_utils.h"

const auto PROGRAM_NAME = "pprsolver";
const auto PROCESSOR_NAME_SINGLE = "SINGLE";
const auto PROCESSOR_NAME_SMP = "SMP";

enum class solution_mode {
    SINGLE,
    SMP,
    OPENCL
};

struct parameters {
    std::string file_name;
    int percentile;
    solution_mode mode;
    cl::Device device;
};

/**
 * Make an attempt to parse program parameters from arguments
 *
 * @param argc argc
 * @param argv argv
 * @param parameters parameters reference to be filled
 * @return boolean flag indicating whether the parameters have been successfully parsed
 */
bool parse_params_from_args(int argc, char *argv[], parameters &parameters);

/**
 * Print the program help message
 */
void print_help();