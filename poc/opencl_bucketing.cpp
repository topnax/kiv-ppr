//
// Created by topnax on 10.10.21.
//

#include <vector>
#include <cmath>
#include <cstdint>
#include "bucketing_utils.h"
#include "solution.h"
#include "opencl_bucketing.h"
#include "opencl_utils.h"
#include "CL/cl.h"
#include "bucketing_constants.h"
#include <iostream>
#include <fstream>
#include "opencl/opencl_sources.h"


std::pair<std::vector<uint64_t>, uint64_t> create_buckets_opencl(char *file_name, cl::Context &context, cl::Device &dev) {
    cl_int error;

    std::vector<uint64_t> buckets(BUCKET_COUNT);
    uint64_t buckets_total_items = 0;

    // open file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    auto buffer_size = NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE_OPENCL;
    // prepare a buffer based on the NUMBER_SIZE_BYTES and NUMBER_BUFFER_SIZE constants
    std::vector<char> buffer(buffer_size, 0);
    auto out_buffer = new uint64_t[NUMBER_BUFFER_SIZE_OPENCL];
    std::unique_ptr<uint64_t[]> out_buffer_ptr(out_buffer);

    cl::Kernel kernel = get_kernel_for_program(opencl_create_buckets_source, "run", context, dev);

    while (true) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        if (fin.gcount() < NUMBER_SIZE_BYTES) break;

        // calculate how many float numbers were read
        auto numbers_read = fin.gcount() / NUMBER_SIZE_BYTES;

        cl::Buffer cl_buf_buffer_vals(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR,
                                      (size_t)fin.gcount(), buffer.data(), &error);
        if (error != CL_SUCCESS) {
            std::wcout << "non 1success: " << error << std::endl;
        }
        cl::Buffer cl_buf_buffer_outs(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, numbers_read * NUMBER_SIZE_BYTES, nullptr, &error);
        if (error != CL_SUCCESS) {
            std::wcout << "non 2success: " << error << std::endl;
        }


        //cl::Kernel kernel(program, "run", &error);
        //std::wcout << "kernel hwp: " << error << std::endl;
        kernel.setArg(0, cl_buf_buffer_vals);
        kernel.setArg(1, cl_buf_buffer_outs);
        // kernel.setArg(2, NUMBER_SHIFT);
        kernel.setArg(2, NUMBER_SHIFT);
        kernel.setArg(3, NUMBER_MAX);

        cl::CommandQueue queue(context, dev);
        error = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numbers_read), 8);
        if (error != CL_SUCCESS) {
            std::wcout << "non 2success: " << error << std::endl;
        }

        error = queue.enqueueReadBuffer(cl_buf_buffer_outs, CL_TRUE, 0, numbers_read * NUMBER_SIZE_BYTES, out_buffer);

        if (error != CL_SUCCESS) {
            std::wcout << "non 5success: " << error << std::endl;
        }
        queue.finish();
        if (error != CL_SUCCESS) {
            std::wcout << "non 6success: " << error << std::endl;
        }

        for (int i = 0; i < numbers_read; i++) {
            if (out_buffer[i] != NUMBER_MAX) {
                buckets[out_buffer[i]] += 1;
                buckets_total_items++;
            }
        }
    }

    return std::pair(buckets, buckets_total_items);
}

