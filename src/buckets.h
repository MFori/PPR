/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <vector>
#include <fstream>
#include "state.h"

#ifndef PPR_BUCKETS_H
#define PPR_BUCKETS_H

const int NUMBER_SIZE_BYTES = 8;
const int BUFFER_SIZE_NUMBERS = 1000;
const int BUCKETS_COUNT = 100;

struct Histogram {
    size_t total_values = 0; // total numbers in this histogram scope
    double value_min = -std::numeric_limits<long>::infinity(); // min value in histogram
    double value_max = std::numeric_limits<long>::infinity(); // max value in histogram
    long file_min = 0; // first valid number from this histogram in file
    long file_max = std::numeric_limits<long>::infinity(); // last valid number from this histogram in file
    long offset_start = 0; // sub-histogram start offset
    long offset_end = 0; // sub-histogram end offset
};

std::pair<double, double> find_histogram_limits(std::ifstream *file, size_t *total_values, State *state);

// create buckets, return buckets
std::vector<long> create_buckets(std::ifstream *file, Histogram *histogram, State *state);

// find bucket with percentile, return bucket min, max values
std::pair<double, double> find_bucket(int percentile);

#endif /* PPR_BUCKETS_H */
