//
// Created by topnax on 26.11.21.
//

#pragma once

#define CL_TARGET_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_LOG_ERRORS stdout

#if __has_include(<CL/opencl.hpp>)
    #include <CL/opencl.hpp>
#else
    #include "opencl.hpp"
#endif

/**
 * Make an attempt to find an OpenCL device by the given name.
 *
 * @param device_name name of the device to be found
 * @param out_device device reference to be written to if the device is find
 *
 * @return `true` when device was found, `false` otherwise
 */
bool cl_get_device(const std::string &device_name, cl::Device &out_device);

/**
 * Compile an OpenCL program
 *
 * @param program_content program itself
 * @param program_name name of the program to be run
 * @param context OpenCL context
 * @param dev OpenCL device to be compiled for
 *
 * @return the compile OpenCl program
 */
cl::Program get_program(const std::string &program_content, const std::string &program_name, cl::Context &context, cl::Device &dev);
