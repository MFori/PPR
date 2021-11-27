/**
 * Created by Martin on 20.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_HISTOGRAM_H
#define PPR_HISTOGRAM_H

#include <fstream>

/**
 * Memory constraint
 */
const unsigned int MEMORY_LIMIT = 200 * 1024 * 1024; // 200MB in bytes
/**
 * Number of doubles to read from file to buffer
 */
const int BUFFER_SIZE_NUMBERS = 80000;

/**
 * Number of bytes of one double number
 */
const int NUMBER_SIZE_BYTES = 8;
/**
 * Number of bits of one double number
 */
const int NUMBER_SIZE_BITS = 8 * 8;

/**
 * Number of bits added each step to bucket_bits
 * @see Histogram::bucket_bits
 */
const int BUCKET_STEP_BITS = 8;
/**
 * Starting bucket bits
 * @see Histogram::bucket_bits
 */
const int BUCKET_BITS_START = 16;
/**
 * Max bucket bits
 * @see Histogram::bucket_bits
 */
const int BUCKET_BITS_MAX = 48;
/**
 * Number of sub buckets
 */
const unsigned long SUB_BUCKETS_COUNT = (unsigned long) pow(2, BUCKET_STEP_BITS);

/**
 * Max number
 */
const unsigned long long MAX_NUMBER = 0xFFFFFFFFFFFFFFFF;
/**
 * Max positive number (sign bit = 0)
 */
const unsigned long long MAX_POSITIVE_NUMBER = 0x7FFFFFFFFFFFFFFF;
/**
 * Max items in bucket to be full processed
 */
const unsigned long MAX_BUCKET_ITEMS = 1000;
/**
 * Bit shift to get sign bit from double
 */
const unsigned int SIGN_SHIFT = 63;

/**
 * Histogram
 */
class Histogram {
public:
    /**
     * Number of total values in this histogram
     */
    size_t total_values = 0;
    /**
     * Minimal value (which can be) in this histogram
     */
    unsigned long long value_min = 0;
    /**
     * Maximal value (which can be) in this histogram
     */
    unsigned long long value_max = MAX_NUMBER;
    /**
     * Position of file where is first number from this histogram
     */
    size_t file_min = 0;
    /**
     * Position of file where is last number from this histogram
     */
    size_t file_max = 0;
    /**
     * Percentile position in this histogram
     */
    size_t percentile_position = 0;

    // empty constructor
    Histogram() {};

    /**
     * Check if histogram contains value
     * @param value value to check
     * @return contains?
     */
    bool contains(double value) const;

    /**
     * Get histogram range
     * @return range
     */
    unsigned long long range() const;

    /**
     * Get bucket index base on number value
     * @param value number value
     * @return bucket index
     */
    unsigned long long bucket_index(double value) const;

    /**
     * Can be histogram shrink?
     * @return can?
     */
    bool can_shrink() const;

    /**
     * Shrink histogram
     * @param buckets
     * @param bucket_index selected bucket witch will be "zoomed" as new histogram configuration
     * @param bucket_percentile_position position of percentile in bucket
     */
    void shrink(const std::vector<long> &buckets, unsigned long long bucket_index, size_t bucket_percentile_position);

    /**
     * Get buckets count
     * @return bucket counts
     */
    unsigned long get_buckets_count() const;

    /**
     * Number of bits of number used to specify bucket index
     * The highest value the more buckets will be in histogram
     */
    unsigned int bucket_bits = BUCKET_BITS_START;
    /**
     * Bucket mask used to get bucket max value from bucket index (=min value)
     */
    unsigned long long bucket_mask = MAX_NUMBER >> bucket_bits;
    /**
     * Number of bit shift (>>) to get bucket index from value
     */
    unsigned int bucket_shift = NUMBER_SIZE_BITS - bucket_bits;
    /**
     * Index of min value in whole histogram (not only shrink part)
     */
    unsigned long long min_index = 0;
    /**
     * Number of buckets
     */
    unsigned long buckets_count = (unsigned long) pow(2, bucket_bits);

private:

    /**
    * Shrink histogram
    * @param bucket_index selected bucket witch will be "zoomed" as new histogram configuration
    */
    void shrink_histogram(unsigned long long bucket_index);
};

#endif /* PPR_HISTOGRAM_H */
