/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "buckets_cl.h"

bool has_cl_device(char *name) {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    for (int i = 0; i < platforms.size(); i++) {
        auto platform = platforms.at(i);
        char name[100];
        platform.getInfo(CL_PLATFORM_NAME, &name);
        std::cout << "openCl platform: " << name << std::endl;
        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        for (int j = 0; j < devices.size(); j++) {
            devices.at(j).getInfo(CL_DEVICE_NAME, name);
            std::cout << "openCl device: " << name << std::endl;
        }
    }

    return false;
}

std::vector<long> create_buckets_cl(std::ifstream *file, Histogram *histogram) {
    std::vector<long> buckets(10);
    return buckets;
}

double get_percentile_value_cl(std::ifstream *file, Histogram *histogram) {
    return 1;
}

std::pair<size_t, size_t> get_value_positions_cl(std::ifstream *file, Histogram *histogram, double value) {
    auto res = std::pair<size_t, size_t>(1, 2);
    return res;
}