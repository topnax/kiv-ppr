//
// Created by topnax on 10.10.21.
//
#pragma comment(lib, "OpenCL.lib")

#include <vector>
#include <cstdint>
#include "opencl_bucketing.h"
#include "opencl_utils.h"
#include "CL/cl.h"
#include "../bucketing/bucketing_constants.h"
#include <iostream>
#include <fstream>
#include "opencl_sources.h"
#include "../utils/watchdog.h"


std::pair<std::vector<uint64_t>, uint64_t> create_buckets_opencl(char *file_name, cl::Context &context, cl::Device &dev) {
    const auto bucket_buffer_size = BUCKET_COUNT * NUMBER_SIZE_BYTES;
    std::vector<uint64_t> buckets(BUCKET_COUNT);
    uint64_t buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    const auto input_buffer_size = NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE_OPENCL;
    std::vector<char> file_buffer(input_buffer_size, 0);

    cl::Program create_buckets_program = get_program(create_buckets_source, "create_buckets", context, dev);

    cl::KernelFunctor<
            cl::Buffer, // data to be processed
            cl::Buffer, // histogram buffer to write results to
            cl_int      // number of bits to be shifted
    > create_buckets_functor(create_buckets_program, "create_buckets");

    cl_int error = 0;
    // prepare an input buffer to which read data will be passed
    cl::Buffer input_buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, (size_t) input_buffer_size, file_buffer.data(), &error);

    if (error != CL_SUCCESS) {
        std::wcout << "OpenCL failed to create the input buffer: " << error << std::endl;
        exit(-1);
    }

    // prepare the buckets buffer
    cl::Buffer buckets_buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, bucket_buffer_size, buckets.data(), &error);
    if (error != CL_SUCCESS) {
        std::wcout << "OpenCL failed to create the buckets buffer: " << error << std::endl;
        exit(-1);
    }

    // prepare the command queue
    cl::CommandQueue queue(context, dev);

    if (error != CL_SUCCESS) {
        std::wcout << "OpenCL failed to write to the buckets buffer: " << error << std::endl;
        exit(-1);
    }

    while (true) {
        ThreadWatchdog::kick();
        // read from the file stream
        fin.read(file_buffer.data(), file_buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        // write read data to the input buffer
        error = queue.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, input_buffer_size, file_buffer.data());
        if (error != CL_SUCCESS) {
            std::wcout << "OpenCL failed to write to the input buffer: " << error << std::endl;
            exit(-1);
        }

        // set enqueue args based on the numbers read
        cl::EnqueueArgs args(queue, cl::NDRange(numbers_read), NUMBER_SIZE_BYTES);

        create_buckets_functor(args, input_buffer, buckets_buffer, NUMBER_SHIFT, error);
        if (error != CL_SUCCESS) {
            std::wcout << "OpenCL failed to invoke the kernel functor: " << error << std::endl;
            exit(-1);
        }
    }

    // read the created histogram back to the main memory
    error = queue.enqueueReadBuffer(buckets_buffer, CL_TRUE, 0, buckets.size() * 8, buckets.data());
    if (error != CL_SUCCESS) {
        std::wcout << "OpenCL computation failed" << error << std::endl;
        exit(-1);
    }

    // just to make sure
    queue.finish();

    // unlike on CPU we have to compute the total number of bucket items here
    for (int i = 0; i < buckets.size(); i++) {
        if (buckets[i] > 0) {
            buckets_total_items += buckets[i];
        }
    }

    return std::pair(buckets, buckets_total_items);
}
