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

// indexes of bucket_index kernel arguments
const unsigned int BUCKET_INDEX_ARG_DATA = 0;
const unsigned int BUCKET_INDEX_ARG_INDEX = 1;
const unsigned int BUCKET_INDEX_ARG_FLAG = 2;
const unsigned int BUCKET_INDEX_ARG_MIN = 3;
const unsigned int BUCKET_INDEX_ARG_MAX = 4;
const unsigned int BUCKET_INDEX_ARG_SHIFT = 5;
const unsigned int BUCKET_INDEX_ARG_OFFSET = 6;

/**
 * ClManager
 */
class ClManager {
public:
    /**
     * Create manager by OpenCl device name
     * @param device_name OpenCl device name
     */
    ClManager(char *device_name);

    ~ClManager() noexcept;

    /**
     * OpenCl context
     */
    cl::Context context;
    /**
     * OpenCl queue
     */
    cl::CommandQueue queue;

    /**
     * OpenCl kernel function bucket_index
     */
    cl::Kernel kernel_bucket_index;

};

/**
 * Initialize OpenCl
 * @param device_name OpenCl device name
 */
void init_cl(char *device_name);

/**
 * Clear memory
 */
void clear_bucketing_cl();

/**
 * create_buckets OpenCl variant
 * @see create_buckets for specification
 */
std::vector<long> create_buckets_cl(std::ifstream *file, Histogram *histogram);

/**
 * get_percentile_value OpenCl variant
 * @see get_percentile_value for specification
 */
double get_percentile_value_cl(std::ifstream *file, Histogram *histogram);

/**
 * get_value_positions OpenCl variant
 * @see get_value_positions for specification
 */
std::pair<size_t, size_t> get_value_positions_cl(std::ifstream *file, Histogram *histogram, double value);


#endif /* PPR_BUCKETS_CL_H */
