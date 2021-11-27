/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_PERCENTILE_FINDER_H
#define PPR_PERCENTILE_FINDER_H

#include "buckets.h"

const unsigned int MAX_STEPS = 10;

struct Result {
    double value;
    size_t first_pos;
    size_t last_pos;
};

void run(char *file_name, int percentile, ProcessorType processor_type, char *cl_device, Result *result);

#endif /* PPR_PERCENTILE_FINDER_H */
