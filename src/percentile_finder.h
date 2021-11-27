/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_PERCENTILE_FINDER_H
#define PPR_PERCENTILE_FINDER_H

#include "buckets.h"

/**
 * Maximum steps of percentile finder
 */
const unsigned int MAX_STEPS = 8;

/**
 * Percentile finder result holder
 */
struct Result {
    /**
     * Found percentile value
     */
    double value;
    /**
     * First position of value in file (number of byte)
     */
    size_t first_pos;
    /**
     * Last position of value in file (number of byte)
     */
    size_t last_pos;
};

/**
 * Run percentile finder
 *
 * @param file_name file name (path) to process
 * @param percentile percentile number 1-100
 * @param processor_type processing type
 * @param cl_device name of OpenCl device or nullptr if processor_type != OpenCl
 * @param result finder result pointer which will be filled with actual result
 */
void run(char *file_name, int percentile, ProcessorType processor_type, char *cl_device, Result *result);

#endif /* PPR_PERCENTILE_FINDER_H */
