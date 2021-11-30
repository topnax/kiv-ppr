//
// Created by topnax on 26.11.21.
//

#pragma once

#define CL_TARGET_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_LOG_ERRORS stdout

// local include instead of the global one (so that the teacher can run it without installing additional header files)
#include "opencl.hpp"

bool cl_get_device(const std::string &device_name, cl::Device &out_device);

cl::Program get_program(const std::string &program_content, const std::string &program_name, cl::Context &context, cl::Device &dev);
