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

// create buckets, return buckets
std::vector<long> create_buckets_single(std::ifstream *file, Histogram *histogram, State *state);

double get_percentile_value_single(std::ifstream *file, Histogram *histogram);

std::pair <size_t, size_t> get_value_positions_single(std::ifstream *file, Histogram *histogram, double value);

#endif /* PPR_BUCKETS_SINGLE_H */
