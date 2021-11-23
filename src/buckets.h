/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <vector>
#include "histogram.h"

#ifndef PPR_BUCKETS_H
#define PPR_BUCKETS_H

enum class ProcessorType {
    Single,
    SMP,
    OpenCL
};

void set_processor_type(ProcessorType processor_type);

long get_percentile_position(int percentile, size_t total_values);

// create buckets, return buckets
std::vector<long> create_buckets(std::ifstream *file, Histogram *histogram, State *state);

// find bucket with percentile, return bucket index
std::pair<size_t, size_t> find_bucket(const std::vector<long> &buckets, Histogram *histogram);

double get_percentile_value(std::ifstream *file, Histogram *histogram);

std::pair<size_t, size_t> get_value_positions(std::ifstream *file, Histogram *histogram, double value);


#endif /* PPR_BUCKETS_H */
