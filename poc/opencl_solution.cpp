//
// Created by topnax on 23.11.21.
//

#define CL_TARGET_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_LOG_ERRORS stdout

#include <CL/cl2.hpp>
#include <CL/cl.h>
#include <iostream>
#include <ostream>

#include "opencl_solution.h"
#include "opencl/opencl_sources.h"


cl::Device cl_get_gpu_device() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (platforms.size() > 0) {
        auto platform = platforms.front();
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

        if (devices.size() > 0)
            return devices.front();
    }

    throw std::runtime_error("Error occurred while selecting the OpenCL GPU device.");
    return cl::Device();
}

solution_result process_file_opencl(char *file_name, int percentile) {
    cl_int error;
    auto dev = cl_get_gpu_device();
    cl::Context context(dev);
    cl::Program program(context, hello_world_code);

    error = program.build("-cl-std=CL2.0");
    if (error != CL_BUILD_SUCCESS) {
        std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
        printf("buildlog: %s\n", buildlog.c_str());
        throw std::runtime_error("Error occurred while building the OpenCL program: " + std::to_string(error));
    }

    uint64_t data[10];
    for (int i = 0; i < 10; i++) {
        data[i] = 10000;
    }

    uint64_t outs[10];
    for (int i = 0; i < 10; i++) {
        outs[i] = 0;
    }

    cl::Buffer cl_buf_buffer_vals(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR,
                                  sizeof(data), data, &error);
    if (error != CL_SUCCESS) {
        std::wcout << "non 1success: " << error << std::endl;
    }
    cl::Buffer cl_buf_buffer_outs(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY, sizeof(outs), nullptr, &error);
    if (error != CL_SUCCESS) {
        std::wcout << "non 2success: " << error << std::endl;
    }


    cl::Kernel kernel(program, "run", &error);
    std::wcout << "kernel hwp: " << error << std::endl;
    kernel.setArg(0, cl_buf_buffer_vals);
    kernel.setArg(1, cl_buf_buffer_outs);

    cl::CommandQueue queue(context, dev);
    error = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(10), 8);
    queue.enqueueReadBuffer(cl_buf_buffer_outs, CL_TRUE, 0, sizeof(outs), outs);
    queue.finish();

    if (error != CL_SUCCESS) {
        std::wcout << "non success: " << error << std::endl;
    } else {

        for (int i = 0; i < 10; i++) {
            std::wcout << i << "x: " << outs[i] << std::endl;
        }
    }

    return solution_result(1, std::pair(1, 1));
}
