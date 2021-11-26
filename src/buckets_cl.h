/**
 * Created by Martin on 26.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_BUCKETS_CL_H
#define PPR_BUCKETS_CL_H

#include <vector>
#include "histogram.h"

#define CL_TARGET_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_LOG_ERRORS stdout
#pragma comment(lib, "OpenCL.lib")

#include <CL/cl.h>
#include <CL/opencl.hpp>

bool has_cl_device(char *name);

std::vector<long> create_buckets_cl(std::ifstream *file, Histogram *histogram);

double get_percentile_value_cl(std::ifstream *file, Histogram *histogram);

std::pair<size_t, size_t> get_value_positions_cl(std::ifstream *file, Histogram *histogram, double value);


#endif /* PPR_BUCKETS_CL_H */
