/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_BUCKETS_CL_H
#define PPR_BUCKETS_CL_H

#include <vector>
#include "histogram.h"
#include "cl_lib.h"

std::vector<long> create_buckets_cl(std::ifstream *file, Histogram *histogram);

double get_percentile_value_cl(std::ifstream *file, Histogram *histogram);

std::pair<size_t, size_t> get_value_positions_cl(std::ifstream *file, Histogram *histogram, double value);


#endif /* PPR_BUCKETS_CL_H */
