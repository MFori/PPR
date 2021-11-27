/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */

#include "utils.h"
#include <iostream>

bool utils::is_valid_double(double d) {
    int cl = std::fpclassify(d);
    return cl == FP_NORMAL || cl == FP_ZERO;
}

size_t utils::get_file_size(std::ifstream *file) {
    file->seekg(0, std::ios::end);
    return (size_t) file->tellg();
}

bool utils::has_cl_device(char *name) {
    auto devices = get_cl_devices();
    for (const auto &device : devices) {
        if (device == name) {
            return true;
        }
    }
    return false;
}

cl::Device utils::get_cl_device(char *name) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for (const auto &platform : platforms) {
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        for (auto &device : devices) {
            std::string device_name = device.getInfo<CL_DEVICE_NAME>();

            if (device_name == name) {
                return device;
            }
        }
    }

    throw std::runtime_error("OpenCL device not found");
}

std::vector<std::string> utils::get_cl_devices() {
    std::vector<std::string> devices_names;
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for (const auto &platform : platforms) {
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        for (auto &device : devices) {
            devices_names.emplace_back(device.getInfo<CL_DEVICE_NAME>().c_str());
        }
    }

    return devices_names;
}
