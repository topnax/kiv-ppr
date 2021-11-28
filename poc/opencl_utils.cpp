//
// Created by topnax on 26.11.21.
//
#include "opencl_utils.h"

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

cl::Kernel get_kernel_for_program(const std::string &program_content, const std::string &program_name, cl::Context &context, cl::Device &dev) {
    cl::Program program(context, program_content);

    auto error = program.build("-cl-std=CL2.0");
    if (error != CL_BUILD_SUCCESS) {
        std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
        printf("buildlog: %s\n", buildlog.c_str());
        throw std::runtime_error("Error occurred while building the OpenCL program: " + std::to_string(error));
    }

    cl::Kernel kernel(program, program_name.c_str(), &error);



    return kernel;
}
