/**
 * Created by Martin on 23.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <algorithm>
#include <map>
#include "buckets_single.h"
#include "utils.h"
#include "watchdog.h"

// create buckets, return buckets
std::vector<long> create_buckets_single(std::ifstream *file, Histogram *histogram) {
    bool has_file_min = false;
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
        Watchdog::kick();

        bool had_valid = false;
        for (int i = 0; i < read; i++) {
            auto value = buffer.at(i);
            if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
            histogram->total_values++;

            auto bucket_index = histogram->bucket_index(value);
            buckets[bucket_index]++;
            had_valid = true;
        }

        Watchdog::kick();
        if (had_valid && !has_file_min) {
            has_file_min = true;
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
        size_t file_position = file->tellg();
        if (file_position >= histogram->file_max) break;
    }

    size_t sum = 0;
    for(auto it = values.begin(); it != values.end(); it++) {
        sum += it->second;
        if(sum > histogram->percentile_position) {
            return it->first;
        }
    }

    return 0;
}

std::pair<size_t, size_t> get_value_positions_single(std::ifstream *file, Histogram *histogram, double value) {
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