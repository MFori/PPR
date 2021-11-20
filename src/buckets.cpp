/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "buckets.h"
#include "utils.h"
#include <vector>
#include <iostream>

std::pair<double, double> find_histogram_limits(std::ifstream *file, size_t *total_values, State *state) {
    double min = std::numeric_limits<double>::infinity();
    double max = -std::numeric_limits<double>::infinity();
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->seekg(0);

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        for (int i = 0; i < read; i++) {
            double value = buffer.at(i);
            if (!utils::is_valid_double(value)) continue;

            (*total_values)++;
            if (value < min) {
                min = value;
            }
            if (value > max) {
                max = value;
            }
        }
    }

    std::pair<double, double> pair(floor(min), ceil(max));
    return pair;
}

inline bool is_in_histogram(Histogram *histogram, double value) {
    return value >= histogram->value_min && value <= histogram->value_max;
}

inline size_t get_bucket_index(double value) {
    auto content = *((unsigned long long *) &value);
    return content >> BUCKET_SHIFT;
}

std::vector<long> create_buckets(std::ifstream *file, Histogram *histogram, State *state) {
    size_t file_min = -1;
    size_t file_max = histogram->file_max;

    std::vector<long> buckets(BUCKETS_COUNT);
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min * NUMBER_SIZE_BYTES);
    histogram->total_values = 0;

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        bool had_valid = false;
        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !is_in_histogram(histogram, value)) continue;
            histogram->total_values++;

            auto bucket_index = get_bucket_index(value);
            buckets[bucket_index]++;
            had_valid = true;
        }

        size_t file_position = file->tellg();
        if (had_valid) {
            if (file_min == -1) file_min = file_position;
            file_max = file_position;
        }

        if (file_position >= histogram->file_max) break;
    }

    histogram->file_min = file_min;
    histogram->file_max = file_max;

    return buckets;
}

inline size_t get_sub_bucket_index(double value, Histogram *histogram) {
    double range = (histogram->value_max + 1) - histogram->value_min;
    size_t bucket_size = range / BUCKETS_COUNT;
    return floor((value - histogram->value_min) / bucket_size);
}

std::vector<long> create_sub_buckets(std::ifstream *file, Histogram *histogram, State *state) {
    size_t file_min = -1;
    size_t file_max = histogram->file_max;

    std::vector<long> buckets(SUB_BUCKETS_COUNT);
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min * NUMBER_SIZE_BYTES);
    histogram->total_values = 0;

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        bool had_valid = false;
        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !is_in_histogram(histogram, value)) continue;
            histogram->total_values++;

            auto bucket_index = get_sub_bucket_index(value, histogram);
            buckets[bucket_index]++;
            had_valid = true;
        }

        size_t file_position = file->tellg();
        if (had_valid) {
            if (file_min == -1) file_min = file_position;
            file_max = file_position;
        }

        if (file_position >= histogram->file_max) break;
    }

    histogram->file_min = file_min;
    histogram->file_max = file_max;

    return buckets;
}

std::pair<double, double>
find_bucket(int percentile, const std::vector<long> &buckets, Histogram *histogram, size_t *bucket_index) {
    long percentile_position = (long) (((double) percentile / 100.0) * histogram->total_values);
    long count = 0;
    *bucket_index = -1;
    size_t index;

    for (index = buckets.size() - 1; index > buckets.size() / 2; index--) {
        count += buckets[index];
        if (count >= percentile_position) {
            *bucket_index = index;
            break;
        }
    }

    if (*bucket_index == -1) {
        for (index = 0; index <= buckets.size() / 2; index++) {
            count += buckets[index];
            if (count >= percentile_position) {
                *bucket_index = index;
                break;
            }
        }
    }

    // todo find limits
    auto limits = std::pair<double, double>(0, 1);
    return limits;
}

size_t find_sub_bucket(int percentile) {
    return -1;
}