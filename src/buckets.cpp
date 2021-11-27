/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "buckets.h"
#include <vector>
#include "buckets_single.h"
#include "buckets_smp.h"
#include "buckets_cl.h"

// global processing type
ProcessorType buckets_proc_type;

void set_processor_type(ProcessorType processor_type, char *cl_device) {
    buckets_proc_type = processor_type;
    if (processor_type == ProcessorType::OpenCL) {
        // init OpenCl context
        init_cl(cl_device);
    }
}

void clear_bucketing() {
    if (buckets_proc_type == ProcessorType::OpenCL) {
        clear_bucketing_cl();
    }
}

long get_percentile_position(int percentile, size_t total_values) {
    if (percentile == 100) return (long) (total_values - 1);
    return (long) (((double) percentile / 100.0) * total_values);
}

std::vector<long> create_buckets(std::ifstream *file, Histogram *histogram) {
    if (buckets_proc_type == ProcessorType::OpenCL) {
        return create_buckets_cl(file, histogram);
    } else if (buckets_proc_type == ProcessorType::SMP) {
        return create_buckets_smp(file, histogram);
    } else {
        return create_buckets_single(file, histogram);
    }
}

std::pair<unsigned long long, size_t> find_bucket(const std::vector<long> &buckets, Histogram *histogram) {
    size_t percentile_position = histogram->percentile_position;
    unsigned long count = 0;
    auto bucket_index = buckets.size() - 1;
    bool found = false;

    unsigned long long full_index = (unsigned long long) bucket_index + histogram->min_index;
    auto full_content = full_index << histogram->bucket_shift;
    bool negative = (full_content >> SIGN_SHIFT) == 1;
    // offset = number of negative indexes to read first
    auto offset = (size_t) (negative ? ((full_content & MAX_POSITIVE_NUMBER) >> histogram->bucket_shift) : 0);

    // start with negative indexes descending
    while (offset > 0) {
        count += buckets.at(bucket_index);
        if (count > percentile_position) {
            found = true;
            break;
        }
        bucket_index--;
        offset--;
    }

    auto remaining = (size_t) bucket_index;
    if (!found) {
        // continue with positive indexes ascending
        for (bucket_index = 0; bucket_index <= remaining; bucket_index++) {
            count += buckets[bucket_index];
            if (count > percentile_position) {
                break;
            }
        }
    }

    size_t bucket_percentile_position = percentile_position - (count - buckets[bucket_index]);
    auto res = std::pair<unsigned long long, size_t>(bucket_index, bucket_percentile_position);
    return res;
}

double get_percentile_value(std::ifstream *file, Histogram *histogram) {
    if (buckets_proc_type == ProcessorType::OpenCL) {
        return get_percentile_value_cl(file, histogram);
    } else if (buckets_proc_type == ProcessorType::SMP) {
        return get_percentile_value_smp(file, histogram);
    } else {
        return get_percentile_value_single(file, histogram);
    }
}

std::pair<size_t, size_t> get_value_positions(std::ifstream *file, Histogram *histogram, double value) {
    if (buckets_proc_type == ProcessorType::OpenCL) {
        return get_value_positions_cl(file, histogram, value);
    } else if (buckets_proc_type == ProcessorType::SMP) {
        return get_value_positions_smp(file, histogram, value);
    } else {
        return get_value_positions_single(file, histogram, value);
    }
}