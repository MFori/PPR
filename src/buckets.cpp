/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "buckets.h"
#include "utils.h"
#include <vector>

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

    std::pair<double, double> pair(min, max);
    return pair;
}

inline bool is_in_histogram(Histogram *histogram, double value) {
    return value >= histogram->value_min && value <= histogram->value_max;
}

std::vector<long> create_buckets(std::ifstream *file, Histogram *histogram, State *state) {
    long file_min = -1;
    long file_max = histogram->file_max;
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
            double value = buffer.at(i);
            if (!utils::is_valid_double(value) || !is_in_histogram(histogram, value)) continue;

            histogram->total_values++;
            int bucket_index = 0;
            // todo calculate bucket index based on buckets count and value_min, value_max
            // todo increment buckets[i]++;
            buckets[i]++;

            had_valid = true;
        }

        long file_position = file->tellg();
        if (had_valid) {
            if (file_min == -1) {
                file_min = file_position;
            }
            file_max = file_position;
        }

        if (file_position >= histogram->file_max) break;
    }

    histogram->file_min = file_min;
    histogram->file_max = file_max;

    return buckets;
}