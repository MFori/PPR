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

    std::vector<double> operator()(tbb::flow_control &fc) const;

private:
    std::ifstream *file;
    Histogram *histogram;

    void init();
};

struct BucketChunk {
    size_t file_min = -1;
    size_t file_max = -1;
    size_t total_values = 0;
    std::vector<long> buckets;
};

class SMPBucketChunksCreator {
public:
    SMPBucketChunksCreator(Histogram *histogram) : histogram(histogram) {};

    BucketChunk operator()(const std::vector<double> &buffer) const;

private:
    Histogram *histogram;
};

class SMPBucketsCreator {
public:
    SMPBucketsCreator(Histogram *histogram, std::vector<long> *buckets) : histogram(histogram), buckets(buckets) {};

    void operator()(const BucketChunk& bucketChunk) const;

private:
    Histogram *histogram;
    std::vector<long> *buckets;
};

class Transform {
public:
    double operator()(double const number) const {
        double answer = 0.0;
        if (number > 0.0)
            answer = number + 1;
        return answer;
    }
};

class DataWriter {
private:
    FILE *my_output;

public:
    DataWriter(FILE *out) :
            my_output{out} {};

    void operator()(double const answer) const {
    }
};

class DataReader {
private:
    FILE *my_input;

public:
    DataReader(FILE *in) :
            my_input{in} {};

    DataReader(const DataReader &a) :
            my_input{a.my_input} {};

    ~DataReader() {};

    double operator()(tbb::flow_control &fc) const {
        double i = 11;
        if (i > 10) {
            fc.stop();
            return 0.0;
        }
        return i;
    }
};

#endif /* PPR_BUCKETS_SMP_H */
