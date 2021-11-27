/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "naive.h"
#include "logging.h"
#include "histogram.h"
#include "utils.h"

std::pair<size_t, size_t> find_percentile_naive(char *file_name, int percentile, double *value) {
    std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);
    if (!file.is_open()) {
        std::pair<long, long> res(0, 0);
        return res;
    }
    std::vector<double> buffer(BUFFER_SIZE_NUMBERS);
    size_t buffer_size_bytes = buffer.size() * NUMBER_SIZE_BYTES;
    file.clear();
    file.seekg(0);

    std::vector<double> numbers;
    while (true) {
        file.read((char *) buffer.data(), buffer_size_bytes);
        auto read = file.gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        for (int i = 0; i < read; i++) {
            auto val = buffer.at(i);
            if (!utils::is_valid_double(val)) continue;
            numbers.push_back(val);
        }
    }

    std::sort(numbers.begin(), numbers.end());
    long n = (long) (((double) percentile / 100) * numbers.size());
    if (percentile == 100) n = (long) (numbers.size() - 1);
    LOG_D("naive position: " << n << ", " << numbers.size());
    double percentile_value = numbers.at(n);

    file.clear();
    file.seekg(0);

    long first = -1;
    long last = 0;
    long pos = 0;

    while (true) {
        file.read((char *) buffer.data(), buffer_size_bytes);
        auto read = file.gcount() / NUMBER_SIZE_BYTES;
        if (read < 1) break;

        for (int i = 1; i < read; i++) {
            auto val = buffer.at(i);

            if (val == percentile_value) {
                if (first == -1) {
                    first = pos;
                }
                last = pos;
            }

            pos++;
        }
    }

    first *= NUMBER_SIZE_BYTES;
    last *= NUMBER_SIZE_BYTES;

    file.close();
    LOG_D("naive:\n" << percentile_value << "\n" << first << "\n" << last);
    *value = percentile_value;
    std::pair<long, long> res(first, last);
    return res;
}