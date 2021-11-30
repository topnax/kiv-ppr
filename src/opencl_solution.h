//
// Created by topnax on 23.11.21.
//
#pragma once

#include "solution.h"

/**
 * Process the file using an OpenCL device for a solution
 * @param file_name file to be processed
 * @param percentile  percentile to be found
 * @param device OpenCL device to be used
 *
 * @return solution result
 */
solution_result process_file_opencl(char *file_name, int percentile, cl::Device &device);
