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

const unsigned int BUCKET_INDEX_ARG_DATA = 0;
const unsigned int BUCKET_INDEX_ARG_INDEX = 1;
const unsigned int BUCKET_INDEX_ARG_FLAG = 2;
const unsigned int BUCKET_INDEX_ARG_MIN = 3;
const unsigned int BUCKET_INDEX_ARG_MAX = 4;
const unsigned int BUCKET_INDEX_ARG_SHIFT = 5;
const unsigned int BUCKET_INDEX_ARG_OFFSET = 6;

class ClManager {
public:
    ClManager(char *device_name);
    ~ClManager() noexcept;

    cl::Context context;
    cl::CommandQueue queue;

    cl::Kernel kernel_bucket_index;

};

void init_cl(char *device_name);

void clear_bucketing_cl();

std::vector<long> create_buckets_cl(std::ifstream *file, Histogram *histogram);

double get_percentile_value_cl(std::ifstream *file, Histogram *histogram);

std::pair<size_t, size_t> get_value_positions_cl(std::ifstream *file, Histogram *histogram, double value);


#endif /* PPR_BUCKETS_CL_H */
