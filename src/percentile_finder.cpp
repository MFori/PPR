/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <fstream>
#include <iostream>
#include "percentile_finder.h"
#include "buckets.h"

void run(char *file_name, int percentile, ProcessorType processor_type, State *state, Result *result) {
    std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);
    size_t total_values = 0;

    //find_percentile_naive(params.file_name, params.percentile);

    // find whole histogram min and max value
    auto limits = find_histogram_limits(&file, &total_values, state);

    // init histogram scope
    struct Histogram histogram{
            .total_values = total_values,
            .value_min = limits.first,
            .value_max = limits.second};

    auto buckets = create_buckets(&file, &histogram, state);
    // todo find bucket index wheres percentile
    // todo if bucket size is large split it again and again and again...
    // todo else find percentile value, and first/last post

    result->value = 0.1;
    result->first_pos = 323;
    result->last_pos = 543;
}