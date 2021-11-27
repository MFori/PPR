/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <fstream>
#include <vector>
#include <algorithm>
#include "naive.h"
#include "logging.h"
#include "histogram.h"
#include "utils.h"

// finding percentile value naive way
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

    // 1. read numbers from file and (valid numbers) push in numbers vector
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

    // 2. sort all numbers
    std::sort(numbers.begin(), numbers.end());
    // 3. get percentile position
    long n = (long) (((double) percentile / 100) * numbers.size());
    if (percentile == 100) n = (long) (numbers.size() - 1);
    LOG_D("naive position: " << n << ", " << numbers.size());

    // 4. get percentile value as number at position
    double percentile_value = numbers.at(n);

    file.clear();
    file.seekg(0);

    long first = -1;
    long last = 0;
    long pos = 0;

    // 5. read file again and find first and last occurrence of value
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