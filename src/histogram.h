/**
 * Created by Martin on 20.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_HISTOGRAM_H
#define PPR_HISTOGRAM_H

#include <fstream>
#include "state.h"

const int BUFFER_SIZE_NUMBERS = 1000;

const int NUMBER_SIZE_BYTES = 8;
const int NUMBER_SIZE_BITS = 8 * 8;

const int BUCKET_BITS = 16;
const unsigned long long BUCKET_MASK = 0x0000FFFFFFFFFFFF;
const unsigned int BUCKET_SHIFT = NUMBER_SIZE_BITS - BUCKET_BITS;
const unsigned long BUCKETS_COUNT = (unsigned long) pow(2, BUCKET_BITS);
const unsigned long SUB_BUCKETS_COUNT = 1000;
const unsigned long MAX_BUCKET_ITEMS = 1000;

class Histogram {
public:
    size_t total_values = 0;
    double value_min = 0;
    double value_max = 0;
    size_t file_min = 0;
    size_t file_max = 0;
    long percentile_position = 0;
    bool is_shrink = false;

    Histogram(State *state) : state(state) {}

    void find_limits(std::ifstream *file);

    bool contains(double value) const;

    double range() const;

    double bucket_size() const;

    size_t bucket_index(double value) const;

    void shrink(const std::vector<long> &buckets, size_t bucket_index, size_t bucket_percentile_position);

    double get_percentile_value(std::ifstream *file);

private:
    State *state;

    void shrink_histogram(size_t bucket_index);

    void shrink_sub_histogram(size_t bucket_index);
};

#endif /* PPR_HISTOGRAM_H */
