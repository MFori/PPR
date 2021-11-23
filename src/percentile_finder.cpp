/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <fstream>
#include "percentile_finder.h"
#include "utils.h"
#include "logging.h"
#include "naive.h"

void run(char *file_name, int percentile, ProcessorType processor_type, State *state, Result *result) {
    //double naive;
    //find_percentile_naive(file_name, percentile, &naive);

    std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);

    Histogram histogram(state);
    auto file_size = utils::get_file_size(&file);
    histogram.file_max = file_size;
    set_processor_type(processor_type);

    std::vector<long> buckets;
    std::pair<size_t, size_t> bucket;

    int step = 0;
    while (true) {
        buckets = create_buckets(&file, &histogram, state);
        if (step == 0) {
            histogram.percentile_position = get_percentile_position(percentile, histogram.total_values);
        }
        LOG_D("total_values: " << histogram.total_values);
        bucket = find_bucket(buckets, &histogram);
        histogram.shrink(buckets, bucket.first, bucket.second);

        LOG_D("total_values: " << histogram.total_values);
        LOG_D("bucket_index: " << bucket.first);
        LOG_D("min: " << *((double *) &histogram.value_min));
        LOG_D("max: " << *((double *) &histogram.value_max));
        LOG_D("range: " << histogram.range());
        LOG_D("position: " << histogram.percentile_position);

        if (buckets[bucket.first] <= MAX_BUCKET_ITEMS || histogram.range() <= 1 ||
            !histogram.can_shrink() || histogram.total_values == 0) {
            break;
        }

        step++;
    }

    if (histogram.range() <= 0 || histogram.total_values == 0) {
        // if histogram min=max take one of them as result value
        result->value = *((double *) &histogram.value_min);
    } else {
        result->value = get_percentile_value(&file, &histogram);
    }

    auto positions = get_value_positions(&file, &histogram, result->value);

    file.close();

    result->first_pos = positions.first;
    result->last_pos = positions.second;
}