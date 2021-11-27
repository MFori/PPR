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
