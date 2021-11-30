//
// Created by topnax on 20.11.21.
//
#pragma once

#include "bucketing_utils.h"

/**
 * Process the file using multiple threads
 * @param file_name file to be processed
 * @param percentile  percentile to be found
 *
 * @return solution result
 */
solution_result process_file_smp(char *file_name, int percentile);
