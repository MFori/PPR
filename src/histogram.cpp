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

void Histogram::find_limits(std::ifstream *file) {
    double min = std::numeric_limits<double>::infinity();
    double max = -std::numeric_limits<double>::infinity();
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->seekg(0);
    total_values = 0;

    while (true) {
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        for (int i = 0; i < read; i++) {
            double value = buffer.at(i);
            if (!utils::is_valid_double(value)) continue;

            total_values++;
            if (value < min) {
                min = value;
            }
            if (value > max) {
                max = value;
            }
        }
    }

    value_min = floor(min);
    value_max = ceil(max);
}

bool Histogram::contains(double value) const {
    return value >= value_min && value <= value_max;
}

double Histogram::range() const {
    return ceil((value_max + 1) - value_min);
}

double Histogram::bucket_size() const {
    return ceil(range() / SUB_BUCKETS_COUNT);
}

size_t Histogram::bucket_index(double value) const {
    if (!is_shrink) {
        auto content = *((unsigned long long *) &value);
        return content >> BUCKET_SHIFT;
    } else {
        return (size_t) floor((value - this->value_min) / this->bucket_size());
    }
}

void Histogram::shrink(const std::vector<long> &buckets, size_t bucket_index, size_t bucket_percentile_position) {
    if (!is_shrink) {
        shrink_histogram(bucket_index);
        is_shrink = true;
    } else {
        shrink_sub_histogram(bucket_index);
    }
    total_values = buckets[bucket_index];
    percentile_position = bucket_percentile_position;
}

void Histogram::shrink_histogram(size_t bucket_index) {
    double bucket_min, bucket_max;
    auto content = *((unsigned long long *) &bucket_index);
    auto d_content = content << BUCKET_SHIFT;
    auto d_content_fill = d_content | BUCKET_MASK;
    auto value = *((double *) &d_content);
    auto value_fill = *((double *) &d_content_fill);

    std::cout << "shrink_histogram: *****" << std::endl;
    std::cout << "index: " << bucket_index << std::endl;
    std::cout << "value: " << value << std::endl;
    std::cout << "value_fill: " << value_fill << std::endl;

    if (value >= 0) {
        bucket_min = value;
        bucket_max = value_fill;
    } else {
        bucket_min = value_fill;
        bucket_max = value;
    }

   // value_min = floor(bucket_min);
   // value_max = ceil(bucket_max);
    value_min = bucket_min;
    value_max = bucket_max;

    std::cout << "shrink min: " << value_min << " index: " << bucket_index << std::endl;
}

void Histogram::shrink_sub_histogram(size_t bucket_index) {
    double bucket_size = this->bucket_size();
    double bucket_min = bucket_index * bucket_size + value_min;
    double bucket_max = bucket_min + bucket_size;

    value_min = bucket_min;
    value_max = bucket_max;
}

double Histogram::get_percentile_value(std::ifstream *file) {
    std::vector<double> values;
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(0 * NUMBER_SIZE_BYTES);

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
        //if (file_position >= file_max) break;
    }

    std::sort(values.begin(), values.end());
    std::cout << "get_value: " << values.size() << " " << percentile_position << std::endl;
    if (percentile_position >= values.size()) percentile_position = values.size() - 1;
    return values[percentile_position];
}