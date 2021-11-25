/**
 * Created by Martin on 20.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <vector>
#include "histogram.h"

bool Histogram::contains(double value) const {
    unsigned long long val = *((unsigned long long *) &value);
    return val >= value_min && val <= value_max;
}

unsigned long long Histogram::range() const {
    return value_max - value_min;
}

size_t Histogram::bucket_index(double value) const {
    auto content = *((unsigned long long *) &value);
    auto index = content >> bucket_shift;
    return index - min_index;
}

void Histogram::shrink(const std::vector<long> &buckets, size_t bucket_index, size_t bucket_percentile_position) {
    shrink_histogram(bucket_index);
    total_values = buckets[bucket_index];
    percentile_position = bucket_percentile_position;
}

bool Histogram::can_shrink() const {
    return bucket_bits <= BUCKET_BITS_MAX;
}

unsigned long Histogram::get_buckets_count() const {
    return buckets_count;
}

void Histogram::shrink_histogram(size_t bucket_index) {
    min_index = value_min >> bucket_shift;
    bucket_index += min_index;

    auto content = *((unsigned long long *) &bucket_index);
    auto d_content = content << bucket_shift;
    auto d_content_fill = d_content | bucket_mask;

    value_min = d_content;
    value_max = d_content_fill;

    bucket_bits += BUCKET_STEP_BITS;
    bucket_mask = MAX_NUMBER >> bucket_bits;
    bucket_shift = NUMBER_SIZE_BITS - bucket_bits;
    buckets_count = SUB_BUCKETS_COUNT;

    min_index = value_min >> bucket_shift;
}