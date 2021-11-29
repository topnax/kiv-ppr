//
// Created by topnax on 26.11.21.
//

#pragma once

#define CL_TARGET_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_LOG_ERRORS stdout

#include "CL/cl2.hpp"

bool cl_get_device(const std::string &device_name, cl::Device &out_device);

cl::Kernel get_kernel_for_program(const std::string &program_content, const std::string &program_name, cl::Context &context, cl::Device &dev);
