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
#include "watchdog.h"

void run(char *file_name, int percentile, ProcessorType processor_type, char *cl_device, Result *result) {
    // open file
    std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);

    Histogram histogram;
    // get file size
    auto file_size = utils::get_file_size(&file);
    histogram.file_max = file_size;
    set_processor_type(processor_type, cl_device);

    std::vector<long> buckets;
    std::pair<unsigned long long, size_t> bucket;

    // main finder loop with 3 operations:
    // 1. creating buckets based od histogram configuration
    // 2. finding bucket with percentile value
    // 3. shrinking histogram to create new histogram configuration
    // repeating until buckets is sufficient small
    unsigned int step = 0;
    while (true) {
        buckets = create_buckets(&file, &histogram);
        if (step == 0) {
            // in first step initialize percentile position (in other steps position is calculate when shrinking histogram)
            histogram.percentile_position = get_percentile_position(percentile, histogram.total_values);
        }
        Watchdog::kick();
        bucket = find_bucket(buckets, &histogram);
        Watchdog::kick();
        histogram.shrink(buckets, bucket.first, bucket.second);
        Watchdog::kick();

        LOG_D("total_values: " << histogram.total_values);
        LOG_D("bucket_index: " << bucket.first);
        LOG_D("min_index: " << histogram.min_index);
        LOG_D("min: " << *((double *) &histogram.value_min));
        LOG_D("max: " << *((double *) &histogram.value_max));
        LOG_D("position: " << histogram.percentile_position);

        if (buckets[(unsigned long) bucket.first] <= MAX_BUCKET_ITEMS || histogram.range() < 1 ||
            !histogram.can_shrink() || histogram.total_values == 0) {
            break;
        }

        if (step >= MAX_STEPS) break;
        step++;
    }

    if (histogram.range() == 0 || histogram.total_values == 0) {
        // if histogram min=max take one of them as result value
        result->value = *((double *) &histogram.value_min);
    } else {
        // get percentile value
        result->value = get_percentile_value(&file, &histogram);
    }
    Watchdog::kick();

    // get first and last position of value in file
    auto positions = get_value_positions(&file, &histogram, result->value);
    Watchdog::kick();

    // allow bucketing to release dynamic memory
    clear_bucketing();
    file.close();

    result->first_pos = positions.first;
    result->last_pos = positions.second;
}