/**
 * Created by Martin on 23.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <algorithm>
#include <tbb/parallel_pipeline.h>
#include "buckets_smp.h"
#include "utils.h"
#include "watchdog.h"

unsigned int max_live_tokens() {
    unsigned int n_cores = std::thread::hardware_concurrency();
    auto max_threads = (MEMORY_LIMIT / (BUFFER_SIZE_NUMBERS * NUMBER_SIZE_BYTES * 3));
    auto max_live_tokens = std::min(n_cores, max_threads);
    return max_live_tokens;
}

std::vector<long> create_buckets_smp(std::ifstream *file, Histogram *histogram) {
    std::vector<long> buckets(histogram->get_buckets_count());
    size_t file_min = 0;
    size_t file_max = 0;

    tbb::parallel_pipeline(max_live_tokens(),
                           tbb::make_filter<void, std::pair<size_t, std::vector<double>>>(
                                   tbb::filter_mode::serial_in_order, SMPFileReader(file, histogram)
                           )
                           &
                           tbb::make_filter<std::pair<size_t, std::vector<double>>, BucketChunk>(
                                   tbb::filter_mode::parallel, SMPBucketChunksCreator(histogram)
                           )
                           &
                           tbb::make_filter<BucketChunk, void>(
                                   tbb::filter_mode::serial_out_of_order,
                                   SMPBucketsCreator(histogram, &buckets, &file_min, &file_max)
                           )
    );

    histogram->file_min = file_min;
    histogram->file_max = file_max;

    return buckets;
}

double get_percentile_value_smp(std::ifstream *file, Histogram *histogram) {
    std::map<double, size_t> values;

    tbb::parallel_pipeline(max_live_tokens(),
                           tbb::make_filter<void, std::pair<size_t, std::vector<double>>>(
                                   tbb::filter_mode::serial_in_order, SMPFileReader(file, histogram)
                           )
                           &
                           tbb::make_filter<std::pair<size_t, std::vector<double>>, std::vector<double>>(
                                   tbb::filter_mode::parallel, SMPValuesExtractor(histogram)
                           )
                           &
                           tbb::make_filter<std::vector<double>, void>(
                                   tbb::filter_mode::serial_out_of_order,
                                   SMPPercentileFinder(&values)
                           )
    );

    size_t sum = 0;
    for(auto it = values.begin(); it != values.end(); it++) {
        sum += it->second;
        if(sum > histogram->percentile_position) {
            return it->first;
        }
    }

    return 0;
}

std::pair<size_t, size_t> get_value_positions_smp(std::ifstream *file, Histogram *histogram, double value) {
    size_t first_position = 0;
    size_t last_position = 0;

    tbb::parallel_pipeline(max_live_tokens(),
                           tbb::make_filter<void, std::pair<size_t, std::vector<double>>>(
                                   tbb::filter_mode::serial_in_order, SMPFileReader(file, histogram)
                           )
                           &
                           tbb::make_filter<std::pair<size_t, std::vector<double>>, PositionsResult>(
                                   tbb::filter_mode::parallel, SMPPositionsExtractor(value)
                           )
                           &
                           tbb::make_filter<PositionsResult, void>(
                                   tbb::filter_mode::serial_out_of_order,
                                   SMPPositionsFinder(&first_position, &last_position)
                           )
    );

    auto positions = std::pair<size_t, size_t>(first_position, last_position);
    return positions;
}

void SMPFileReader::init() {
    file->clear();
    file->seekg(histogram->file_min);
    histogram->total_values = 0;
}

std::pair<size_t, std::vector<double>> SMPFileReader::operator()(tbb::flow_control &fc) const {
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;

    size_t file_position = (size_t) file->tellg();
    if (file_position >= histogram->file_max) {
        fc.stop();
    }

    file->read((char *) buffer.data(), buffer_size_bytes);
    auto read = file->gcount() / NUMBER_SIZE_BYTES;
    if (read < 1) {
        fc.stop();
    }

    Watchdog::kick();

    auto result_buffer = std::vector<double>(buffer.begin(), buffer.begin() + read);
    auto result = std::pair<size_t, std::vector<double>>(file_position, result_buffer);
    return result;
}

BucketChunk SMPBucketChunksCreator::operator()(const std::pair<size_t, const std::vector<double>> &params) const {
    auto buffer = params.second;
    auto read = buffer.size();
    size_t file_position = params.first;
    std::vector<long> buckets(histogram->get_buckets_count());

    BucketChunk chunk{};
    chunk.file_min = 0;
    chunk.file_max = histogram->file_max;
    chunk.total_values = 0;
    chunk.had_valid = false;

    for (int i = 0; i < read; i++) {
        auto value = buffer.at(i);
        if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
        chunk.total_values++;

        auto bucket_index = histogram->bucket_index(value);
        buckets[bucket_index]++;
        chunk.had_valid = true;
    }

    chunk.file_min = file_position;
    chunk.file_max = file_position + (BUFFER_SIZE_NUMBERS * NUMBER_SIZE_BYTES);

    Watchdog::kick();

    chunk.buckets = buckets;
    return chunk;
}

void SMPBucketsCreator::operator()(const BucketChunk &bucketChunk) const {
    histogram->total_values += bucketChunk.total_values;

    if (bucketChunk.had_valid && (bucketChunk.file_min < *file_min || !*has_min)) {
        *has_min = true;
        *file_min = bucketChunk.file_min;
    }
    if (bucketChunk.file_max > *file_max) {
        *file_max = bucketChunk.file_max;
    }

    std::transform(
            buckets->begin(),
            buckets->end(),
            bucketChunk.buckets.begin(),
            buckets->begin(),
            std::plus<>()
    );

    Watchdog::kick();
}

std::vector<double> SMPValuesExtractor::operator()(const std::pair<size_t, const std::vector<double>> &params) const {
    std::vector<double> values;
    auto buffer = params.second;
    auto read = buffer.size();

    for (int i = 0; i < read; i++) {
        auto value = buffer.at(i);
        if (!utils::is_valid_double((double) value) || !histogram->contains(value)) continue;
        values.push_back(value);
    }

    Watchdog::kick();

    return values;
}

void SMPPercentileFinder::operator()(const std::vector<double> &values) const {
    for (auto value : values) {
        auto it = m_values->find(value);
        if (it != m_values->end()) {
            it->second++;
        } else {
            m_values->insert(std::make_pair(value, 1));
        }
    }
}

PositionsResult SMPPositionsExtractor::operator()(const std::pair<size_t, const std::vector<double>> &params) const {
    PositionsResult result{};
    result.has_first = false;
    result.first = 0;
    result.last = 0;

    auto buffer = params.second;
    auto read = buffer.size();
    size_t file_position = params.first;

    for (int i = 0; i < read; i++) {
        auto val = buffer.at(i);
        if (val == value) {
            if (!result.has_first) {
                result.first = file_position + i * NUMBER_SIZE_BYTES;
                result.has_first = true;
            }
            result.last = file_position + i * NUMBER_SIZE_BYTES;
        }
    }

    Watchdog::kick();
    return result;
}

void SMPPositionsFinder::operator()(const PositionsResult &position) const {
    if (position.has_first && (position.first < *first_position || !*has_first == 0)) {
        *first_position = position.first;
        *has_first = true;
    }
    if (position.last > *last_position) {
        *last_position = position.last;
    }
}