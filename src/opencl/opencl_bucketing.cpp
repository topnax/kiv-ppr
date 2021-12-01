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
    std::vector<cl_ulong> buckets(BUCKET_COUNT, 0);
    uint64_t buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    auto buffer_size = NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE_OPENCL;
    std::vector<char> buffer(buffer_size, 0);

    cl::Program create_buckets_program = get_program(create_buckets_source, "create_buckets", context, dev);

    cl::KernelFunctor<
            cl::Buffer, // data to be processed
            cl::Buffer, // histogram buffer to write results to
            cl_int      // number of bits to be shifted
    > create_buckets_functor(create_buckets_program, "create_buckets");

    cl_int error;
    // prepare an input buffer to which read data will be passed
    cl::Buffer input_buffer(context, CL_MEM_READ_WRITE, buffer_size, nullptr, &error);
    if (error != CL_SUCCESS) {
        std::wcout << "OpenCL buffer 1 setup failed" << error << std::endl;
        exit(-1);
    }

    // prepare the buckets buffer
    cl::Buffer buckets_buffer(context, CL_MEM_READ_WRITE, BUCKET_COUNT * NUMBER_SIZE_BYTES, nullptr, &error);
    if (error != CL_SUCCESS) {
        std::wcout << "OpenCL buffer 2 setup failed" << error << std::endl;
        exit(-1);
    }


    // prepare the command queue
    cl::CommandQueue queue(context, dev);

    // setup the buckets buffer
    error = queue.enqueueWriteBuffer(buckets_buffer, CL_TRUE, 0, BUCKET_COUNT * NUMBER_SIZE_BYTES, buckets.data());

    if (error != CL_SUCCESS) {
        std::wcout << "OpenCL buffer setup failed" << error << std::endl;
        exit(-1);
    }

    while (true) {
        ThreadWatchdog::kick();
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        std::wcout << "Calling EWB..." << error << std::endl;
        // write read data to the input buffer
        error = queue.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, buffer_size, buffer.data());
        if (error != CL_SUCCESS) {
            std::wcout << "OpenCL failed to write to the input buffer" << error << std::endl;
            exit(-1);
        }
        std::wcout << "Done EWB..." << error << std::endl;

        // set enqueue args based on the numbers read
        cl::EnqueueArgs args(queue, cl::NDRange(numbers_read));

        std::wcout << "Calling functor..." << error << std::endl;
        create_buckets_functor(args, input_buffer, buckets_buffer, NUMBER_SHIFT, error);
        if (error != CL_SUCCESS) {
            std::wcout << "OpenCL failed to execute kernel" << error << std::endl;
            exit(-1);
        }
        std::wcout << "Functor called..." << error << std::endl;
    }

    
    std::wcout << "About to ERB..." << error << std::endl;
    // read the created histogram back to the main memory
    error = queue.enqueueReadBuffer(buckets_buffer, CL_TRUE, 0, buckets.size() * 8, buckets.data());
    std::wcout << "ERB done..." << error << std::endl;
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
