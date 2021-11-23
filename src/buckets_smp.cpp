/**
 * Created by Martin on 23.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <algorithm>
#include <tbb/parallel_pipeline.h>
#include <iostream>
#include "buckets_smp.h"
#include "utils.h"

std::vector<long> create_buckets_smp(std::ifstream *file, Histogram *histogram, State *state) {
    std::vector<long> buckets(histogram->get_buckets_count());

    tbb::parallel_pipeline(MAX_LIVE_TOKENS,
                           tbb::make_filter<void, std::vector<double>>(
                                   tbb::filter_mode::serial_in_order, SMPFileReader(file, histogram)
                           )
                           &
                           tbb::make_filter<std::vector<double>, BucketChunk>(
                                   tbb::filter_mode::parallel, SMPBucketChunksCreator(histogram)
                           )
                           &
                           tbb::make_filter<BucketChunk, void>(
                                   tbb::filter_mode::serial_out_of_order, SMPBucketsCreator(histogram, &buckets)
                           )
    );

    return buckets;
}

double get_percentile_value_smp(std::ifstream *file, Histogram *histogram) {
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

std::pair<size_t, size_t> get_value_positions_smp(std::ifstream *file, Histogram *histogram, double value) {
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    file->clear();
    file->seekg(histogram->file_min);

    size_t first_position = -1;
    size_t last_position = -1;

    while (true) {
        size_t file_position = file->tellg();
        file->read((char *) buffer.data(), buffer_size_bytes);
        auto read = file->gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        for (int i = 0; i < read; i++) {
            auto val = buffer.at(i);
            if (val == value) {
                if (first_position == -1) {
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

void SMPFileReader::init() {
    file->clear();
    file->seekg(histogram->file_min);
    histogram->total_values = 0;
}

std::vector<double> SMPFileReader::operator()(tbb::flow_control &fc) const {
    //std::cout << "file reader " << std::endl;
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    size_t file_position = file->tellg();
    if (file_position >= histogram->file_max) {
        fc.stop();
    }

    file->read((char *) buffer.data(), buffer_size_bytes);
    auto read = file->gcount() / NUMBER_SIZE_BYTES;
    if (read < 1) {
        fc.stop();
    }

    auto result = std::vector<double>(buffer.begin(), buffer.begin() + read);
    //std::cout << "read: " << read << " " << result.size() << " " << buffer.size() << std::endl;
    //auto result = std::pair<size_t, std::vector<double>>(read, buffer);
    return result;
}

BucketChunk SMPBucketChunksCreator::operator()(const std::vector<double> &buffer) const {
    //std::cout << "chunk creator " << std::endl;
    auto read = buffer.size();
    //std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    std::vector<long> buckets(histogram->get_buckets_count());

    BucketChunk chunk{};
    chunk.file_min = -1;
    chunk.file_max = histogram->file_max;
    chunk.total_values = 0;

    bool had_valid = false;
    for (int i = 0; i < read; i++) {
        auto value = buffer.at(i);
        if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
        chunk.total_values++;

        auto bucket_index = histogram->bucket_index(value);
        buckets[bucket_index]++;
        had_valid = true;
    }

    chunk.buckets = buckets;
    return chunk;
}

void SMPBucketsCreator::operator()(const BucketChunk& bucketChunk) const {
    histogram->total_values += bucketChunk.total_values;

    std::transform(
            buckets->begin(),
            buckets->end(),
            bucketChunk.buckets.begin(),
            buckets->begin(),
            std::plus<>()
    );
}