//
// Created by topnax on 26.11.21.
//
#include <iostream>
#include "opencl_utils.h"
#include <algorithm>

/**
 * Checks whether the given device is an Intel device or not (thanks to @Spiffyk for this function)
 * @param dev device to be checked
 * @return boolean flag indicating whether the given device is an Intel device or not
 */
bool is_intel(cl::Device dev) {
    std::string vendor(dev.getInfo<CL_DEVICE_VENDOR>());
    std::transform(vendor.begin(), vendor.end(), vendor.begin(), ::tolower);
    return vendor.find("intel") != std::string::npos;
}

bool cl_get_device(const std::string &device_name, cl::Device &out_device) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // iterate over all platforms
    for (const auto &platform: platforms) {
        std::vector<cl::Device> devices;

        // accept all devices
        platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

        // check whether the given device was found
        for (const auto &device: devices) {
            std::string name = device.getInfo<CL_DEVICE_NAME>();
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (device_name == name) {
                // device found
                out_device = device;
                return true;
            }
        }
    }

    // device was not found
    return false;
}

cl::Program get_program(const std::string &program_content, const std::string &program_name, cl::Context &context,
                        cl::Device &dev) {
    cl::Program program(context, program_content);

    std::string build_flags = "-cl-std=CL2.0";
    // optimizations are disabled for Intel, because their chips yield incorrect results otherwise and I don't
    // own any device with Intel HD GPU, so I am not able to debug it
    if (is_intel(dev)) {
        build_flags += " -cl-opt-disable";
    }

    auto error = program.build(build_flags.c_str());
    if (error != CL_BUILD_SUCCESS) {
        std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
        printf("Build log: %s\n", buildlog.c_str());
        throw std::runtime_error("Error occurred while building the OpenCL program: " + std::to_string(error));
    }

    return program;
}
