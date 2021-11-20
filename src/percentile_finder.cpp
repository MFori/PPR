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
#include "utils.h"

void run(char *file_name, int percentile, ProcessorType processor_type, State *state, Result *result) {
    std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);
    size_t total_values = 0;

    //find_percentile_naive(params.file_name, params.percentile);

    auto file_size = utils::get_file_size(&file);

    // find whole histogram min and max value
    auto limits = find_histogram_limits(&file, &total_values, state);

    std::cout << "total_values: " << total_values << std::endl;

    // init histogram scope
    struct Histogram histogram{total_values, limits.first, limits.second};
    histogram.file_min = 0;
    histogram.file_max = file_size;

    auto buckets = create_buckets(&file, &histogram, state);
    std::cout << "total_values: " << histogram.total_values << std::endl;

    size_t bucket_index = -1;
    limits = find_bucket(percentile, buckets, &histogram, &bucket_index);
    histogram.value_min = limits.first;
    histogram.value_max = limits.second;

    std::cout << "bucket_index: " << bucket_index << std::endl;
    std::cout << "bucket_size: " << buckets[bucket_index] << std::endl;


    // todo in loop:
    //buckets = create_sub_buckets(&file, &histogram, state);
    // todo if bucket size is large split it again and again and again...
    // todo else find percentile value, and first/last post

    result->value = 0.1;
    result->first_pos = 323;
    result->last_pos = 543;
}