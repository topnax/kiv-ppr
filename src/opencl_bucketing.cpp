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

    cl::Program create_buckets_program = get_program(create_buckets_source, "create_buckets", context, dev);

    cl::KernelFunctor<
            cl::Buffer, // data to be processed
            cl::Buffer, // histogram buffer to write results to
            cl_int      // number of bits to be shifted
    > create_buckets_functor(create_buckets_program, "create_buckets");

    //
    cl_int error;
    // prepare an input buffer to which read data will be passed
    cl::Buffer input_buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                            buffer.size(), buffer.data(), &error);

    // prepare the buckets buffer
    cl::Buffer buckets_buffer(context, CL_MEM_READ_WRITE, buckets.size() * NUMBER_SIZE_BYTES, nullptr, &error);

    // prepare the command queue
    cl::CommandQueue queue(context, dev);

    // setup the buckets buffer
    error = queue.enqueueWriteBuffer(buckets_buffer, CL_TRUE, 0, buckets.size() * NUMBER_SIZE_BYTES, buckets.data());

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

        // write read data to the input buffer
        error = queue.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, numbers_read * NUMBER_SIZE_BYTES, buffer.data());
        if (error != CL_SUCCESS) {
            std::wcout << "OpenCL failed to write to the input buffer" << error << std::endl;
            exit(-1);
        }

        // set enqueue args based on the numbers read
        cl::EnqueueArgs args(queue, cl::NDRange(numbers_read), NUMBER_SIZE_BYTES);

        create_buckets_functor(args, input_buffer, buckets_buffer, NUMBER_SHIFT);
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
