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

ProcessorType buckets_proc_type;

void set_processor_type(ProcessorType processor_type) {
    buckets_proc_type = processor_type;
}


long get_percentile_position(int percentile, size_t total_values) {
    if (percentile == 100) return (long) (total_values - 1);
    return (long) (((double) percentile / 100.0) * total_values);
}

std::vector<long> create_buckets(std::ifstream *file, Histogram *histogram) {
    if(buckets_proc_type == ProcessorType::OpenCL) {
        return create_buckets_cl(file, histogram);
    } else if(buckets_proc_type == ProcessorType::SMP) {
        return create_buckets_smp(file, histogram);
    } else {
        return create_buckets_single(file, histogram);
    }
}

std::pair<size_t, size_t> find_bucket(const std::vector<long> &buckets, Histogram *histogram) {
    size_t percentile_position = histogram->percentile_position;
    unsigned long count = 0;
    size_t bucket_index;
    bool found = false;

    for (bucket_index = buckets.size() - 1; bucket_index >= buckets.size() / 2; bucket_index--) {
        count += buckets[bucket_index];
        if (count > percentile_position) {
            found = true;
            break;
        }
    }

    if (!found) {
        for (bucket_index = 0; bucket_index < buckets.size() / 2; bucket_index++) {
            count += buckets[bucket_index];
            if (count > percentile_position) {
                break;
            }
        }
    }

    size_t bucket_percentile_position = percentile_position - (count - buckets[bucket_index]);
    auto res = std::pair<size_t, size_t>(bucket_index, bucket_percentile_position);
    return res;
}

double get_percentile_value(std::ifstream *file, Histogram *histogram) {
    if(buckets_proc_type == ProcessorType::OpenCL) {
        return get_percentile_value_cl(file, histogram);
    } else if(buckets_proc_type == ProcessorType::SMP) {
        return get_percentile_value_smp(file, histogram);
    } else {
        return get_percentile_value_single(file, histogram);
    }
}

std::pair<size_t, size_t> get_value_positions(std::ifstream *file, Histogram *histogram, double value) {
    if(buckets_proc_type == ProcessorType::OpenCL) {
        return get_value_positions_cl(file, histogram, value);
    } else if(buckets_proc_type == ProcessorType::SMP) {
        return get_value_positions_smp(file, histogram, value);
    } else {
        return get_value_positions_single(file, histogram, value);
    }
}