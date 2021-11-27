/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include <map>
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
    error = program.build(std::vector<cl::Device>{device});

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
    bool has_file_min = false;
    size_t file_min = 0;
    size_t file_max = histogram->file_max;

    cl_int error;
    std::vector<long> buckets(histogram->get_buckets_count());
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min);
    histogram->total_values = 0;

    std::vector<uint32_t> indexes(BUFFER_SIZE_NUMBERS);
    std::vector<char> flags(BUFFER_SIZE_NUMBERS);

    cl::Buffer data_buffer(manager->context, CL_MEM_READ_WRITE, buffer_size_bytes, nullptr, &error);
    cl::Buffer indexes_buffer(manager->context, CL_MEM_READ_WRITE, indexes.size() * sizeof(uint32_t), nullptr, &error);
    cl::Buffer flags_buffer(manager->context, CL_MEM_READ_WRITE, flags.size() * sizeof(bool), nullptr, &error);

    error = manager->kernel_bucket_index.setArg(BUCKET_INDEX_ARG_DATA, data_buffer);
    error = manager->kernel_bucket_index.setArg(BUCKET_INDEX_ARG_INDEX, indexes_buffer);
    error = manager->kernel_bucket_index.setArg(BUCKET_INDEX_ARG_FLAG, flags_buffer);
    error = manager->kernel_bucket_index.setArg(BUCKET_INDEX_ARG_MIN, histogram->value_min);
    error = manager->kernel_bucket_index.setArg(BUCKET_INDEX_ARG_MAX, histogram->value_max);
    error = manager->kernel_bucket_index.setArg(BUCKET_INDEX_ARG_SHIFT, histogram->bucket_shift);
    error = manager->kernel_bucket_index.setArg(BUCKET_INDEX_ARG_OFFSET, histogram->min_index);
    Watchdog::kick();

    while (true) {
        size_t file_position = (size_t) file->tellg();
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;
        Watchdog::kick();

        error = manager->queue.enqueueWriteBuffer(data_buffer, CL_TRUE, 0, (cl::size_type) read * sizeof(double), buffer.data());
        error = manager->queue.enqueueNDRangeKernel(manager->kernel_bucket_index, cl::NullRange, cl::NDRange((cl::size_type) read));
        error = manager->queue.enqueueReadBuffer(indexes_buffer, CL_TRUE, 0, (cl::size_type) read * sizeof(uint32_t), indexes.data());
        error = manager->queue.enqueueReadBuffer(flags_buffer, CL_TRUE, 0, (cl::size_type) read * sizeof(char), (bool *) flags.data());

        bool had_valid = false;
        for (int i = 0; i < read; i++) {
            auto flag = (bool) flags.at(i);
            auto bucket_index = indexes.at(i);
            buckets[bucket_index] += flag;
            histogram->total_values += flag;
            had_valid |= flag;
        }

        Watchdog::kick();
        if (had_valid && !has_file_min) {
            has_file_min = true;
            file_min = file_position;
        }
        file_position = (size_t) file->tellg();
        if (had_valid) file_max = file_position;

        if (file_position >= histogram->file_max) break;
    }

    histogram->file_min = file_min;
    histogram->file_max = file_max;

    return buckets;
}

double get_percentile_value_cl(std::ifstream *file, Histogram *histogram) {
    std::map<double, size_t> values;
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
            auto it = values.find(value);
            if (it != values.end()) {
                it->second++;
            } else {
                values.insert(std::make_pair(value, 1));
            }
        }

        Watchdog::kick();
        size_t file_position = (size_t) file->tellg();
        if (file_position >= histogram->file_max) break;
    }

    size_t sum = 0;
    for (auto it = values.begin(); it != values.end(); it++) {
        sum += it->second;
        if (sum > histogram->percentile_position) {
            return it->first;
        }
    }

    return 0;
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
        size_t file_position = (size_t) file->tellg();
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