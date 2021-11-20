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

const int BUFFER_SIZE_NUMBERS = 1000;

const int NUMBER_SIZE_BYTES = 8;
const int NUMBER_SIZE_BITS = 8 * 8;

const int BUCKET_BITS = 15;
const unsigned int BUCKET_SHIFT = NUMBER_SIZE_BITS - BUCKET_BITS;
const unsigned long BUCKETS_COUNT = pow(2, BUCKET_BITS);
const unsigned long SUB_BUCKETS_COUNT = 1000;

struct Histogram {
    size_t total_values = 0; // total numbers in this histogram scope
    double value_min = 0; // min value in histogram
    double value_max = 0; // max value in histogram
    size_t file_min = 0; // first valid number from this histogram in file
    size_t file_max = 0; // last valid number from this histogram in file
    long offset_start = 0; // sub-histogram start offset
    long offset_end = 0; // sub-histogram end offset
};

std::pair<double, double> find_histogram_limits(std::ifstream *file, size_t *total_values, State *state);

// create buckets, return buckets
std::vector<long> create_buckets(std::ifstream *file, Histogram *histogram, State *state);

std::vector<long> create_sub_buckets(std::ifstream *file, Histogram *histogram, State *state);

// find bucket with percentile, return bucket index
std::pair<double, double>
find_bucket(int percentile, const std::vector<long> &buckets, Histogram *histogram, size_t *bucket_index);

size_t find_sub_bucket(int percentile, std::vector<long> buckets, Histogram *histogram);

inline size_t get_bucket_index(double value, Histogram *histogram);

inline size_t get_sub_bucket_index(double value, Histogram *histogram);

#endif /* PPR_BUCKETS_H */
