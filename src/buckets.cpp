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

long get_percentile_position(int percentile, size_t total_values) {
    if (percentile == 100) return (long) (total_values - 1);
    return (long) (((double) percentile / 100.0) * total_values);
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
            if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
            histogram->total_values++;

            auto bucket_index = histogram->bucket_index(value);
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

std::vector<long> create_sub_buckets(std::ifstream *file, Histogram *histogram, State *state) {
    size_t file_min = -1;
    size_t file_max = histogram->file_max;

    std::vector<long> buckets(SUB_BUCKETS_COUNT);
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    //todo file->seekg(histogram->file_min * NUMBER_SIZE_BYTES);
    file->seekg(0 * NUMBER_SIZE_BYTES);
    bool ct = histogram->total_values == 4;
    histogram->total_values = 0;

    double prev;
    bool has_prev = false;
    bool same = true;
    double min, max;
    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        bool had_valid = false;
        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
            histogram->total_values++;

            if (ct) {
                std::cout << "4: " << value << std::endl;
            }

            if (has_prev && same) {
                same = prev == value;
            }
            if (!has_prev) {
                min = value;
                max = value;
            } else {
                if (value < min) min = value;
                if (value > max) max = value;
            }

            prev = value;
            has_prev = true;

            auto bucket_index = histogram->bucket_index(value);
            if (bucket_index >= buckets.size()) {
                std::cout << "shrink " << histogram->is_shrink << std::endl;
                std::cout << "value: " << value << " min: " << histogram->value_min << " max: " << histogram->value_max
                          << " size: " << histogram->bucket_size() << " range: " << histogram->range() << std::endl;
                std::cout << " > " << bucket_index << ", " << buckets.size() << std::endl;
            }
            buckets[bucket_index]++;
            had_valid = true;
        }

        size_t file_position = file->tellg();
        if (had_valid) {
            if (file_min == -1) file_min = file_position;
            file_max = file_position;
        }

        //if (file_position >= histogram->file_max) break;
    }

    std::cout << "same: " << same << std::endl;
    //std::cout << "min: " << min << std::endl;
    //std::cout << "max: " << max << std::endl;

    histogram->file_min = file_min;
    histogram->file_max = file_max;

    return buckets;
}

std::pair<size_t, size_t> find_bucket(const std::vector<long> &buckets, Histogram *histogram) {
    long percentile_position = histogram->percentile_position;
    long count = 0;
    size_t bucket_index;
    bool found = false;

    for (bucket_index = buckets.size() - 1; bucket_index > buckets.size() / 2; bucket_index--) {
        count += buckets[bucket_index];
        if (count > percentile_position) {
            found = true;
            break;
        }
    }

    if (!found) {
        for (bucket_index = 0; bucket_index <= buckets.size() / 2; bucket_index++) {
            count += buckets[bucket_index];
            if (count > percentile_position) {
                break;
            }
        }
    }

    size_t bucket_percentile_position = percentile_position - (count - buckets[bucket_index]);
    auto res = std::pair<size_t, size_t>(bucket_index, bucket_percentile_position);
    return res;
}

std::pair<size_t, size_t> find_sub_bucket(const std::vector<long> &buckets, Histogram *histogram) {
    long percentile_position = histogram->percentile_position;
    std::cout << "p_pos: " << percentile_position << std::endl;
    long count = 0;
    size_t bucket_index;

    for (bucket_index = 0; bucket_index < buckets.size(); bucket_index++) {
        count += buckets[bucket_index];
        if (count > percentile_position) {
            break;
        }
    }

    if (bucket_index >= buckets.size()) {
        std::cout << "HERE" << buckets.size() << std::endl;
        bucket_index = buckets.size() - 1;
        while (buckets[bucket_index] == 0 && bucket_index > 0) {
            bucket_index--;
            std::cout << "DOWN " << bucket_index << std::endl;
        }
    }

    std::cout << "count: " << count << std::endl;
    std::cout << "buckets: " << buckets.size() << ", index: " << bucket_index << std::endl;
    size_t bucket_percentile_position = percentile_position - (count - buckets[bucket_index]);
    auto res = std::pair<size_t, size_t>(bucket_index, bucket_percentile_position);
    return res;
}