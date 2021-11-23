/**
 * Created by Martin on 23.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_BUCKETS_SMP_H
#define PPR_BUCKETS_SMP_H

#include <vector>
#include "histogram.h"
#include <fstream>
#include <tbb/flow_graph.h>

const unsigned int MAX_LIVE_TOKENS = 4;

std::vector<long> create_buckets_smp(std::ifstream *file, Histogram *histogram, State *state);

double get_percentile_value_smp(std::ifstream *file, Histogram *histogram);

std::pair<size_t, size_t> get_value_positions_smp(std::ifstream *file, Histogram *histogram, double value);

class SMPFileReader {
public:
    SMPFileReader(std::ifstream *file, Histogram *histogram) : file(file), histogram(histogram) { init(); };

    ~SMPFileReader() {};

    std::pair<size_t, std::vector<double>> operator()(tbb::flow_control &fc) const;

private:
    std::ifstream *file;
    Histogram *histogram;

    void init();
};

struct BucketChunk {
    size_t file_min = 0;
    size_t file_max = 0;
    size_t total_values = 0;
    std::vector<long> buckets;
};

class SMPBucketChunksCreator {
public:
    SMPBucketChunksCreator(Histogram *histogram) : histogram(histogram) {};

    BucketChunk operator()(const std::pair<size_t, const std::vector<double>> &params) const;

private:
    Histogram *histogram;
};

class SMPBucketsCreator {
public:
    SMPBucketsCreator(Histogram *histogram, std::vector<long> *buckets, size_t *file_min, size_t *file_max) : histogram(
            histogram), buckets(buckets), file_min(file_min), file_max(file_max) {};

    void operator()(const BucketChunk &bucketChunk) const;

private:
    Histogram *histogram;
    std::vector<long> *buckets;
    size_t *file_min;
    size_t *file_max;
};

class SMPValuesExtractor {
public:
    SMPValuesExtractor(Histogram *histogram) : histogram(histogram) {};

    std::vector<double> operator()(const std::pair<size_t, const std::vector<double>> &params) const;

private:
    Histogram *histogram;
};

class SMPPercentileFinder {
public:
    SMPPercentileFinder(std::vector<double> *values) : m_values(values) {};

    void operator()(const std::vector<double> &values) const;

private:
    std::vector<double> *m_values;
};

#endif /* PPR_BUCKETS_SMP_H */
