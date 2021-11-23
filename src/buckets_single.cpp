/**
 * Created by Martin on 23.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <algorithm>
#include <iostream>
#include "buckets_single.h"
#include "utils.h"

// create buckets, return buckets
std::vector<long> create_buckets_single(std::ifstream *file, Histogram *histogram, State *state) {
    size_t file_min = 0;
    size_t file_max = histogram->file_max;

    std::vector<long> buckets(histogram->get_buckets_count());
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min);
    histogram->total_values = 0;

    while (true) {
        size_t file_position = file->tellg();
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        bool had_valid = false;
        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
            histogram->total_values++;

            auto bucket_index = histogram->bucket_index(value);
            buckets[bucket_index]++;
            had_valid = true;
        }

        if (had_valid && file_min == 0) {
            file_min = file_position;
        }
        file_position = file->tellg();
        if (had_valid) file_max = file_position;

        if (file_position >= histogram->file_max) break;
    }

    histogram->file_min = file_min;
    histogram->file_max = file_max;

    return buckets;
}

double get_percentile_value_single(std::ifstream *file, Histogram *histogram) {
    std::vector<double> values;
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min);

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
            values.push_back(value);
        }

        size_t file_position = file->tellg();
        if (file_position >= histogram->file_max) break;
    }

    std::sort(values.begin(), values.end());
    if (histogram->percentile_position >= values.size()) histogram->percentile_position = values.size() - 1;
    return values[histogram->percentile_position];
}

std::pair<size_t, size_t> get_value_positions_single(std::ifstream *file, Histogram *histogram, double value) {
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min);

    size_t first_position = 0;
    size_t last_position = 0;

    while (true) {
        size_t file_position = file->tellg();
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        for (int i = 0; i < read; i++) {
            auto val = buffer.at(i);
            if (val == value) {
                if (first_position == 0) {
                    first_position = file_position + i * NUMBER_SIZE_BYTES;
                }
                last_position = file_position + i * NUMBER_SIZE_BYTES;
            }
        }

        if (file_position >= histogram->file_max) break;
    }

    auto positions = std::pair<size_t, size_t>(first_position, last_position);
    return positions;
}