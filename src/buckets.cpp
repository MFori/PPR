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

    std::vector<long> buckets(histogram->get_buckets_count());
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min);
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

std::pair<size_t, size_t> find_bucket(const std::vector<long> &buckets, Histogram *histogram) {
    size_t percentile_position = histogram->percentile_position;
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