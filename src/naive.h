/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_NAIVE_H
#define PPR_NAIVE_H

#include <vector>

/**
 * Find percentile value naive way by sorting all numbers in file
 * @param file_name file path to process
 * @param percentile percentile number 1-100
 * @param value will be filled with percentile value
 * @return pair of first and last position of value in file
 */
std::pair<size_t, size_t> find_percentile_naive(char *file_name, int percentile, double *value);

#endif /* PPR_NAIVE_H */
