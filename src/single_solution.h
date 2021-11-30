//
// Created by topnax on 23.11.21.
//
#pragma once

#include "bucketing_utils.h"

/**
 * Process the file using a single thread
 * @param file_name file to be processed
 * @param percentile  percentile to be found
 *
 * @return solution result
 */
solution_result process_file_single(char *file_name, int percentile);
