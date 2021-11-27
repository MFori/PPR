/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <vector>
#include "histogram.h"

#ifndef PPR_BUCKETS_H
#define PPR_BUCKETS_H

/**
 * Processing type
 */
enum class ProcessorType {
    /**
     * Single thread
     */
    Single,
    /**
     * Symmetric multiprocessor
     */
    SMP,
    /**
     * OpenCl
     */
    OpenCL
};

/**
 * Set processing type, may be called before any other bucketing function
 * @param processor_type processing type
 * @param cl_device OpenCl device name
 */
void set_processor_type(ProcessorType processor_type, char *cl_device);

/**
 * Clear bucketing memory, may be called after bucketing stuff
 */
void clear_bucketing();

/**
 * Get percentile position based on percentile number and total number of values
 * @param percentile percentile number 1-10
 * @param total_values number of total values
 * @return percentile position
 */
long get_percentile_position(int percentile, size_t total_values);

/**
 * Create buckets
 * buckets are created as vector of longs, where bucket is specified by index and long value
 * at this index represents number of values in this bucket
 *
 * @param file file stream
 * @param histogram
 * @return buckets
 */
std::vector<long> create_buckets(std::ifstream *file, Histogram *histogram);

/**
 * Find bucket based on histograms configuration (percentile position)
 * @param buckets @see create_buckets for buckets desc
 * @param histogram
 * @return pair of bucket index and percentile position in found bucket
 */
std::pair<unsigned long long, size_t> find_bucket(const std::vector<long> &buckets, Histogram *histogram);

/**
 * Get percentile value base on histogram configuration
 * @param file file stream
 * @param histogram
 * @return percentile value
 */
double get_percentile_value(std::ifstream *file, Histogram *histogram);

/**
 * Get first and last position of value in file
 * @param file file stream
 * @param histogram
 * @param value
 * @return pair of first and last position in file (byte number)
 */
std::pair<size_t, size_t> get_value_positions(std::ifstream *file, Histogram *histogram, double value);


#endif /* PPR_BUCKETS_H */
