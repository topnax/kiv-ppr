//
// Created by topnax on 28.11.21.
//
#pragma once

#include <memory>
#include "opencl_utils.h"

const auto PROGRAM_NAME = "poc";
const auto PROCESSOR_NAME_SINGLE = "single";
const auto PROCESSOR_NAME_SMP = "smp";

enum class solution_mode {
    SERIAL,
    SMP,
    OPENCL
};

struct parameters {
    std::string file_name;
    int percentile;
    solution_mode mode;
    cl::Device device;
};

bool parse_params_from_args(int argc, char *argv[], parameters &parameters);

void print_help();