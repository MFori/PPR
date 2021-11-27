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
#include <map>

/**
 * create_buckets smp variant
 * @see create_buckets for specification
 */
std::vector<long> create_buckets_smp(std::ifstream *file, Histogram *histogram);

/**
 * get_percentile_value smp variant
 * @see get_percentile_value for specification
 */
double get_percentile_value_smp(std::ifstream *file, Histogram *histogram);

/**
 * get_value_positions smp variant
 * @see get_value_positions for specification
 */
std::pair<size_t, size_t> get_value_positions_smp(std::ifstream *file, Histogram *histogram, double value);

/**
 * SMPFileReader
 * Body for parallel pipeline stage
 * Read buffer from file and pass to next stage of pipeline
 */
class SMPFileReader {
public:
    /**
     * Create SMPFileReader
     * @param file file stream
     * @param histogram histogram
     */
    SMPFileReader(std::ifstream *file, Histogram *histogram) : file(file), histogram(histogram) { init(); };

    ~SMPFileReader() {};

    /**
     * Called by pipeline
     * @param fc pipeline flow control
     * @return pair of file position and buffer with raed data
     */
    std::pair<size_t, std::vector<double>> operator()(tbb::flow_control &fc) const;

private:
    /**
     * File stream
     */
    std::ifstream *file;
    /**
     * Histogram
     */
    Histogram *histogram;

    /**
     * Initialize FileReader, called from constructor
     */
    void init();
};

/**
 * Bucket chunk holder
 */
struct BucketChunk {
    /**
     * File position of first value in this bucket chunk
     */
    size_t file_min = 0;
    /**
     * File position of last value in this bucket chunk
     */
    size_t file_max = 0;
    /**
     * Number of total values in this bucket chunk
     */
    size_t total_values = 0;
    /**
     * Buckets
     */
    std::vector<long> buckets;
};

/**
 * SMPBucketChunksCreator
 * Body for parallel pipeline stage
 * Get buffer from previous stage, create bucket chunk and pass it to next stage
 */
class SMPBucketChunksCreator {
public:
    /**
     * Create SMPBucketChunksCreator
     * @param histogram
     */
    SMPBucketChunksCreator(Histogram *histogram) : histogram(histogram) {};

    /**
     * Called by pipeline
     * @param params pair of file position and buffer with raed data
     * @return bucket chunk
     */
    BucketChunk operator()(const std::pair<size_t, const std::vector<double>> &params) const;

private:
    /**
     * Histogram
     */
    Histogram *histogram;
};

/**
 * SMPBucketsCreator
 * Body for parallel pipeline stage
 * Get bucket chunk and add it to buckets, last stage
 */
class SMPBucketsCreator {
public:
    /**
     * Create SMPBucketsCreator
     * @param histogram
     * @param buckets empty buckets which will be filled after pipeline finish
     * @param file_min will be filled with file position of first item in histogram
     * @param file_max will be filled with file position of last item in histogram
     */
    SMPBucketsCreator(Histogram *histogram, std::vector<long> *buckets, size_t *file_min, size_t *file_max) : histogram(
            histogram), buckets(buckets), file_min(file_min), file_max(file_max) {};

    /**
     * Called by pipeline
     * @param bucketChunk
     */
    void operator()(const BucketChunk &bucketChunk) const;

private:
    /**
     * Histogram
     */
    Histogram *histogram;
    /**
     * Buckets, may contains whole histograms buckets after pipeline finish
     */
    std::vector<long> *buckets;
    /**
     * Will be filled with file position of first item in histogram
     */
    size_t *file_min;
    /**
     * Will be filled with file position of last item in histogram
     */
    size_t *file_max;
    /**
    * Has min file_min value (or is default 0)
    */
    bool has_min_val = false;
    // ptr to be accessible in const method
    bool *has_min = &has_min_val;
};

/**
 * SMPValuesExtractor
 * Body for parallel pipeline stage
 * Read all values from buffer, get only valid for histogram scope, and pass it to next stage
 */
class SMPValuesExtractor {
public:
    /**
     * Create SMPValuesExtractor
     * @param histogram
     */
    SMPValuesExtractor(Histogram *histogram) : histogram(histogram) {};

    /**
     * Called by pipeline
     * @param params pair of file position and buffer
     * @return valid values
     */
    std::vector<double> operator()(const std::pair<size_t, const std::vector<double>> &params) const;

private:
    /**
     * Histogram
     */
    Histogram *histogram;
};

/**
 * SMPPercentileFinder
 * Body for parallel pipeline stage
 * Gets values and join it to values of whole histogram
 */
class SMPPercentileFinder {
public:
    /**
     * Create SMPPercentileFinder
     * @param values will be filled with values of whole histogram after pipeline finish
     */
    SMPPercentileFinder(std::map<double, size_t> *values) : m_values(values) {};

    /**
     * Called by pipeline
     * @param values valid values
     */
    void operator()(const std::vector<double> &values) const;

private:
    /**
     * Values will be filled with values of whole histogram after pipeline finish
     */
    std::map<double, size_t> *m_values;
};

/**
 * Position result holder
 */
struct PositionsResult {
    /**
     * First position in file
     */
    size_t first;
    /**
     * Last position in file
     */
    size_t last;
    /**
     * Has first position (or default value)
     */
    bool has_first;
};

/**
 * SMPPositionsExtractor
 * Body for parallel pipeline stage
 * Gets pair of file position and buffer, found positions of value and pass it to next stage
 */
class SMPPositionsExtractor {
public:
    /**
     * Create SMPPositionsExtractor
     * @param value to find positions of
     */
    SMPPositionsExtractor(double value) : value(value) {};

    /**
     * Called by pipeline
     * @param params pair of file position and buffer
     * @return positions
     */
    PositionsResult operator()(const std::pair<size_t, const std::vector<double>> &params) const;

private:
    /**
     * Value to find position of
     */
    double value;
};

/**
 * SMPPositionsFinder
 * Body for parallel pipeline stage
 * Get position result and find position in whole histogram
 */
class SMPPositionsFinder {
public:
    /**
     * Create SMPPositionsFinder
     * @param first_position will be filled of first position of value
     * @param last_position will be filled of last position of value
     */
    SMPPositionsFinder(size_t *first_position, size_t *last_position) : first_position(first_position),
                                                                        last_position(last_position) {};

    /**
     * Called by pipeline
     * @param position
     */
    void operator()(const PositionsResult &position) const;

private:
    /**
     * First position of value in whole histogram
     */
    size_t *first_position;
    /**
     * Last position of value in whole histogram
     */
    size_t *last_position;
    /**
     * Has first position (or default 0)
     */
    bool has_first_val = false;
    // ptr to access in const method
    bool *has_first = &has_first_val;
};

#endif /* PPR_BUCKETS_SMP_H */
