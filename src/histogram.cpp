/**
 * Created by Martin on 20.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <vector>
#include <iostream>
#include <algorithm>
#include "histogram.h"
#include "utils.h"

bool Histogram::contains(double value) const {
    unsigned long long val = *((unsigned long long *) &value);
    return val >= value_min && val <= value_max;
}

unsigned long long Histogram::range() const {
    return value_max - value_min;
}

size_t Histogram::bucket_index(double value) const {
    auto content = *((unsigned long long *) &value);
    auto index = content >> bucket_shift;
    return index - min_index;
}

void Histogram::shrink(const std::vector<long> &buckets, size_t bucket_index, size_t bucket_percentile_position) {
    shrink_histogram(bucket_index);
    total_values = buckets[bucket_index];
    percentile_position = bucket_percentile_position;
}

bool Histogram::can_shrink() const {
    return bucket_bits <= BUCKET_BITS_MAX;
}

unsigned long Histogram::get_buckets_count() const {
    return buckets_count;
}

void Histogram::shrink_histogram(size_t bucket_index) {
    auto min_content = *((unsigned long long *) &value_min);
    min_index = min_content >> bucket_shift;
    bucket_index += min_index;

    auto content = *((unsigned long long *) &bucket_index);
    auto d_content = content << bucket_shift;
    auto d_content_fill = d_content | bucket_mask;

    value_min = d_content;
    value_max = d_content_fill;

    bucket_bits += BUCKET_STEP_BITS;
    bucket_mask = MAX_NUMBER >> bucket_bits;
    bucket_shift = NUMBER_SIZE_BITS - bucket_bits;
    buckets_count = SUB_BUCKETS_COUNT;

    min_content = *((unsigned long long *) &value_min);
    min_index = min_content >> bucket_shift;
}

double Histogram::get_percentile_value(std::ifstream *file) {
    std::vector<double> values;
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(file_min);

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !contains(value)) continue;
            values.push_back(value);
        }

        size_t file_position = file->tellg();
        if (file_position >= file_max) break;
    }

    std::sort(values.begin(), values.end());
    if (percentile_position >= values.size()) percentile_position = values.size() - 1;
    return values[percentile_position];
}

std::pair<size_t, size_t> Histogram::get_value_positions(std::ifstream *file, double value) const {
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(file_min);

    size_t first_position = -1;
    size_t last_position = -1;

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        size_t file_position = file->tellg();

        for (int i = 0; i < read; i++) {
            auto val = buffer.at(i);
            if (val == value) {
                if (first_position == -1) {
                    first_position = file_position;
                }
                last_position = file_position;
            }
        }

        if (file_position >= file_max) break;
    }

    auto positions = std::pair<size_t, size_t>(first_position, last_position);
    return positions;
}