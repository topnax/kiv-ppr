//
// Created by topnax on 10.10.21.
//
#pragma comment(lib, "OpenCL.lib")

#include <vector>
#include <cstdint>
#include "opencl_bucketing.h"
#include "opencl_utils.h"
#include "CL/cl.h"
#include "bucketing_constants.h"
#include <iostream>
#include <fstream>
#include "opencl/opencl_sources.h"
#include "watchdog.h"


inline void compute_bucket_indices(std::vector<char> &data, uint64_t data_size, std::vector<uint64_t> &out, cl::Context &context, cl::Device &dev, cl::Kernel &kernel) {
    cl_int error;
    // prepare the input buffer
    cl::Buffer cl_buf_buffer_vals(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR,
                                  data_size, data.data(), &error);

    // TODO might be able to directly write into the input buffer to save some memory space?
    // prepare the out buffer
    cl::Buffer cl_buf_buffer_outs(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, data_size, nullptr, &error);

    // set kernel arguments
    kernel.setArg(0, cl_buf_buffer_vals);
    kernel.setArg(1, cl_buf_buffer_outs);
    kernel.setArg(2, NUMBER_SHIFT);
    kernel.setArg(3, NUMBER_MAX);

    // TODO might be able to reuse the queue?
    // prepare the command queue
    cl::CommandQueue queue(context, dev);

    error = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(data_size / NUMBER_SIZE_BYTES), 8);
    error = queue.enqueueReadBuffer(cl_buf_buffer_outs, CL_TRUE, 0, data_size, out.data());
    queue.finish();

    if (error != CL_SUCCESS) {
        std::wcout << "OpenCL computation failed" << error << std::endl;
        exit(-1);
    }
}

std::pair<std::vector<uint64_t>, uint64_t> create_buckets_opencl(char *file_name, cl::Context &context, cl::Device &dev) {
    std::vector<uint64_t> buckets(BUCKET_COUNT);
    uint64_t buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    auto buffer_size = NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE_OPENCL;
    std::vector<char> buffer(buffer_size, 0);
    std::vector<uint64_t> out_buffer(NUMBER_BUFFER_SIZE_OPENCL);

    cl::Kernel kernel = get_kernel_for_program(compute_buckets_indices_program, "run", context, dev);

    while (true) {
        ThreadWatchdog::kick();
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        // compute bucket indices on the OpenCL device
        compute_bucket_indices(buffer, fin.gcount(), out_buffer, context, dev, kernel);

        for (int i = 0; i < numbers_read; i++) {
            if (out_buffer[i] != NUMBER_MAX) {
                buckets[out_buffer[i]] += 1;
                buckets_total_items++;
            }
        }
    }

    return std::pair(buckets, buckets_total_items);
}

std::pair<double, std::pair<uint64_t, uint64_t>>
find_percentile_value_opencl(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name, cl::Context &context, cl::Device &dev) {
    std::map<double, std::unique_ptr<bucket_item>> numbers_in_bucket;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::binary);

    uint64_t index = 0;

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    auto buffer_size = NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE_OPENCL;
    std::vector<char> buffer(buffer_size, 0);
    std::vector<uint64_t> out_buffer(NUMBER_BUFFER_SIZE_OPENCL);

    cl::Kernel kernel = get_kernel_for_program(compute_buckets_indices_program, "run", context, dev);

    while (true) {
        ThreadWatchdog::kick();
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        uint64_t numbers_read = static_cast<uint64_t>(fin.gcount() / NUMBER_SIZE_BYTES);

        compute_bucket_indices(buffer, fin.gcount(), out_buffer, context, dev, kernel);

        for (uint64_t i = 0; i < numbers_read; i++) {
            auto bucket_index = out_buffer[i];

            if (out_buffer[i] != NUMBER_MAX) {
                if (bucket_index == bucket) {
                    update_histogram_item(*((uint64_t * )&buffer[i * 8]), numbers_in_bucket, index);
                }
            }
            index++;
        }
    }

    return find_percentile_in_histogram(percentile_position_in_bucket, numbers_in_bucket);
}

