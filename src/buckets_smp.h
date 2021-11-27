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

std::vector<long> create_buckets_smp(std::ifstream *file, Histogram *histogram);

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
    bool had_valid = false;
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
    bool has_min_val = false;
    bool *has_min = &has_min_val;
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

struct PositionsResult {
    size_t first;
    size_t last;
    bool has_first;
};

class SMPPositionsExtractor {
public:
    SMPPositionsExtractor(double value) : value(value) {};

    PositionsResult operator()(const std::pair<size_t, const std::vector<double>> &params) const;

private:
    double value;
};

class SMPPositionsFinder {
public:
    SMPPositionsFinder(size_t *first_position, size_t *last_position) : first_position(first_position),
                                                                        last_position(last_position) {};

    void operator()(const PositionsResult &position) const;

private:
    size_t *first_position;
    size_t *last_position;
};

#endif /* PPR_BUCKETS_SMP_H */
