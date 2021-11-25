/**
 * Created by Martin on 20.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_HISTOGRAM_H
#define PPR_HISTOGRAM_H

#include <fstream>

const int BUFFER_SIZE_NUMBERS = 50000;

const int NUMBER_SIZE_BYTES = 8;
const int NUMBER_SIZE_BITS = 8 * 8;

const int BUCKET_STEP_BITS = 4;
const int BUCKET_BITS_START = 16;
const int BUCKET_BITS_MAX = 48;
const unsigned long SUB_BUCKETS_COUNT = (unsigned long) pow(2, BUCKET_STEP_BITS);

const unsigned long long MAX_NUMBER = 0xFFFFFFFFFFFFFFFF;
const unsigned long MAX_BUCKET_ITEMS = 1000;

class Histogram {
public:
    size_t total_values = 0;
    unsigned long long value_min = 0;
    unsigned long long value_max = MAX_NUMBER;
    size_t file_min = 0;
    size_t file_max = 0;
    size_t percentile_position = 0;

    Histogram() {};

    bool contains(double value) const;

    unsigned long long range() const;

    size_t bucket_index(double value) const;

    bool can_shrink() const;

    void shrink(const std::vector<long> &buckets, size_t bucket_index, size_t bucket_percentile_position);

    unsigned long get_buckets_count() const;

private:
    unsigned int bucket_bits = BUCKET_BITS_START;
    unsigned long long bucket_mask = MAX_NUMBER >> bucket_bits;
    unsigned int bucket_shift = NUMBER_SIZE_BITS - bucket_bits;
    unsigned long long min_index = 0;
    unsigned long buckets_count = (unsigned long) pow(2, bucket_bits);

    void shrink_histogram(size_t bucket_index);
};

#endif /* PPR_HISTOGRAM_H */
