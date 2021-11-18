/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <vector>

#ifndef PPR_BUCKETS_H
#define PPR_BUCKETS_H

const int BUCKETS_COUNT = 100;

// create buckets, return buckets, total items
std::pair<std::vector<long>, long> create_buckets(char *file_name);

// find bucket with percentile, return bucket min, max values
std::pair<double, double> find_bucket(int percentile);

#endif /* PPR_BUCKETS_H */
