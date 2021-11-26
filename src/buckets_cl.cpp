/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "buckets_cl.h"

std::vector<long> create_buckets_cl(std::ifstream *file, Histogram *histogram) {
    std::vector<long> buckets(10);
    return buckets;
}

double get_percentile_value_cl(std::ifstream *file, Histogram *histogram) {
    return 1;
}

std::pair<size_t, size_t> get_value_positions_cl(std::ifstream *file, Histogram *histogram, double value) {
    auto res = std::pair<size_t, size_t>(1, 2);
    return res;
}