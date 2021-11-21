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
#include "naive.h"

void run(char *file_name, int percentile, ProcessorType processor_type, State *state, Result *result) {
    //double naive;
    //find_percentile_naive(file_name, percentile, &naive);
    std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);

    std::cout << "buckets: " << BUCKETS_COUNT << std::endl;

    Histogram histogram(state);
    auto file_size = utils::get_file_size(&file);
    histogram.file_max = file_size;

    // find whole histogram min and max value
    histogram.find_limits(&file);
    std::cout << "total_values: " << histogram.total_values << std::endl;

    auto buckets = create_buckets(&file, &histogram, state);
    histogram.percentile_position = get_percentile_position(percentile, histogram.total_values);
    auto bucket = find_bucket(buckets, &histogram);
    histogram.shrink(buckets, bucket.first, bucket.second);

    std::cout << "total_values: " << histogram.total_values << std::endl;
    std::cout << "bucket_index: " << bucket.first << std::endl;
    std::cout << "bucket_size: " << buckets[bucket.first] << std::endl;

    double range = histogram.range();
    while (true) {
        std::cout << "total_values: " << histogram.total_values << std::endl;
        std::cout << "bucket_size: " << buckets[bucket.first] << std::endl;
        buckets = create_sub_buckets(&file, &histogram, state);

        std::cout << "total_values: " << histogram.total_values << std::endl;
        std::cout << "bucket_index: " << bucket.first << std::endl;
        std::cout << "min: " << histogram.value_min << std::endl;
        std::cout << "max: " << histogram.value_max << std::endl;
        std::cout << "range: " << histogram.range() << std::endl;
        std::cout << "position: " << histogram.percentile_position << std::endl;

        bucket = find_sub_bucket(buckets, &histogram);
        if(bucket.second >= buckets[bucket.first]) {
            std::cout << "MAY BE ERROR" << std::endl;
        }
        histogram.shrink(buckets, bucket.first, bucket.second);

        if (buckets[bucket.first] <= MAX_BUCKET_ITEMS || histogram.range() <= 0) {
            break;
        }
        if(range == histogram.range()) {
            std::cout << "break range same" << std::endl;
            break;
        }
        range = histogram.range();
    }

    if (histogram.range() <= 0) {
        // if histogram min=max take one of them as result value
        result->value = histogram.value_min;
    } else {
        result->value = histogram.get_percentile_value(&file);
    }

    file.close();
    std::cout << std::endl << "total_values: " << histogram.total_values << std::endl;
    std::cout << "bucket_index: " << bucket.first << std::endl;
    //std::cout << "bucket_size: " << buckets[bucket.first] << std::endl;

    result->first_pos = 323;
    result->last_pos = 543;
}