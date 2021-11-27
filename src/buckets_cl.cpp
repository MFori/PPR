/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include <set>
#include "buckets_cl.h"
#include "utils.h"
#include "opencl_src.h"
#include "watchdog.h"

ClManager *manager = nullptr;

void init_cl(char *device_name) {
    manager = new ClManager(device_name);
}

void clear_bucketing_cl() {
    delete manager;
}

ClManager::ClManager(char *device_name) {
    auto device = utils::get_cl_device(device_name);
    cl_int error;
    context = cl::Context(device);
    cl::Program program(context, cl_program_src);
    error = program.build(std::vector{device});

    if (error != CL_BUILD_SUCCESS) {
        std::cerr << "[ERROR] OpenCL program build error: " << error << std::endl
                  << "Log:" << std::endl
                  << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;

        throw std::runtime_error("Error while building the OpenCL program: " + std::to_string(error));
    }

    queue = cl::CommandQueue(context, device, 0, &error);
    if (error != CL_SUCCESS) {
        throw std::runtime_error("Error while creating the OpenCL command queue: " + std::to_string(error));
    }

    kernel_bucket_index = cl::Kernel(program, "get_bucket_index", &error);
}

ClManager::~ClManager() noexcept {
    queue.finish();
}

std::vector<long> create_buckets_cl(std::ifstream *file, Histogram *histogram) {
    cl_int error;
    std::vector<long> buckets(histogram->get_buckets_count());
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(0);
    histogram->total_values = 0;

    std::vector<uint32_t> indexes(BUFFER_SIZE_NUMBERS);

    cl::Buffer data(manager->context, CL_MEM_READ_WRITE, buffer_size_bytes, nullptr, &error);
    cl::Buffer indexes_buf(manager->context, CL_MEM_READ_WRITE, indexes.size() * sizeof(uint32_t), nullptr, &error);

    error = manager->kernel_bucket_index.setArg(0, data);
    error = manager->kernel_bucket_index.setArg(1, indexes_buf);
    error = manager->kernel_bucket_index.setArg(2, histogram->value_min);
    error = manager->kernel_bucket_index.setArg(3, histogram->value_max);
    error = manager->kernel_bucket_index.setArg(4, histogram->bucket_shift);
    error = manager->kernel_bucket_index.setArg(5, histogram->min_index);

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        error = manager->queue.enqueueWriteBuffer(data, CL_TRUE, 0, buffer.size() * sizeof(double), buffer.data());
        error = manager->queue.enqueueNDRangeKernel(manager->kernel_bucket_index, cl::NullRange, cl::NDRange(read));
        error = manager->queue.enqueueReadBuffer(indexes_buf, CL_TRUE, 0, indexes.size() * sizeof(uint32_t),
                                                 indexes.data());

        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
            histogram->total_values++;

            //auto bucket_index = histogram->bucket_index(value);
            auto bucket_index = indexes[i];
            buckets[bucket_index]++;
        }
    }

    return buckets;
}

double get_percentile_value_cl(std::ifstream *file, Histogram *histogram) {
    std::multiset<double> values;
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min);

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;
        Watchdog::kick();

        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
            values.insert(value);
        }

        Watchdog::kick();
        size_t file_position = file->tellg();
        if (file_position >= histogram->file_max) break;
    }

    return *std::next(values.begin(), histogram->percentile_position);
}

std::pair<size_t, size_t> get_value_positions_cl(std::ifstream *file, Histogram *histogram, double value) {
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min);

    bool has_first_position = false;
    size_t first_position = 0;
    size_t last_position = 0;

    while (true) {
        size_t file_position = file->tellg();
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;
        Watchdog::kick();

        for (int i = 0; i < read; i++) {
            auto val = buffer.at(i);
            if (val == value) {
                if (!has_first_position) {
                    has_first_position = true;
                    first_position = file_position + i * NUMBER_SIZE_BYTES;
                }
                last_position = file_position + i * NUMBER_SIZE_BYTES;
            }
        }

        Watchdog::kick();
        if (file_position >= histogram->file_max) break;
    }

    auto positions = std::pair<size_t, size_t>(first_position, last_position);
    return positions;
}