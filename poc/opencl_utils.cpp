//
// Created by topnax on 26.11.21.
//
#include <iostream>
#include "opencl_utils.h"

cl::Device cl_get_device(const std::string &device_name) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (!platforms.empty()) {
        auto platform = platforms.front();
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        for (const auto &device: devices)  {
            std::string name = device.getInfo<CL_DEVICE_NAME>();
            if (device_name == name) {
                return device;
            }
        }
    }

    std::wcout << "Could not find the OpenCL device identified by the name of \"" << device_name.c_str() << "\". Aborting." << std::endl;
    exit(-1);
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
