/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <fstream>
#include <iostream>
#include "percentile_finder.h"
#include "buckets.h"

void run(char *file_name, int percentile, ProcessorType processor_type, State *state, Result *result) {
    std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);
    size_t total_values = 0;

    auto limits = find_histogram_limits(&file, &total_values);
    std::cout << "limits: " << limits.first << " " << limits.second << "\nread: " << total_values << "\n";
    file.close();

    //find_percentile_naive(params.file_name, params.percentile);

    result->value = 0.1;
    result->first_pos = 323;
    result->last_pos = 543;
}