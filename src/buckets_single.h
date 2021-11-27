/**
 * Created by Martin on 23.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_BUCKETS_SINGLE_H
#define PPR_BUCKETS_SINGLE_H

#include <vector>
#include "histogram.h"

/**
 * create_buckets single thread variant
 * @see create_buckets for specification
 */
std::vector<long> create_buckets_single(std::ifstream *file, Histogram *histogram);

/**
 * get_percentile_value single thread variant
 * @see get_percentile_value for specification
 */
double get_percentile_value_single(std::ifstream *file, Histogram *histogram);

/**
 * get_value_positions single thread variant
 * @see get_value_positions for specification
 */
std::pair <size_t, size_t> get_value_positions_single(std::ifstream *file, Histogram *histogram, double value);

#endif /* PPR_BUCKETS_SINGLE_H */
