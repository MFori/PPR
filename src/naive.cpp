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

std::pair<long, long> find_percentile_naive(char *file_name, int percentile, double *value) {
    std::ifstream file(file_name, std::ifstream::in | std::ifstream::binary);
    if (!file.is_open()) {
        std::pair<long, long> res(0, 0);
        return res;
    }
    std::vector<char> buffer(8000);

    std::vector<double> numbers;
    while (true) {
        file.read(buffer.data(), buffer.size());
        auto read = file.gcount() / 8;
        if (read < 1) break;

        for (int i = 0; i < read; i++) {
            double value = ((double *) buffer.data())[i];
            numbers.push_back(value);
        }
    }

    std::sort(numbers.begin(), numbers.end());
    long n = (long) (((float) percentile / 100) * numbers.size());
    if (percentile == 100) n = (long) (numbers.size() - 1);
    double percentile_value = numbers.at(n);

    file.clear();
    file.seekg(0);

    long first = -1;
    long last = 0;
    long pos = 0;

    while (true) {
        file.read(buffer.data(), buffer.size());
        auto read = file.gcount() / 8;
        if (read < 1) break;

        for (int i = 1; i < read; i++) {
            double value = ((double *) buffer.data())[i];

            if (value == percentile_value) {
                if (first == -1) {
                    first = pos;
                }
                last = pos;
            }

            pos++;
        }
    }

    file.close();
    std::cout << "naive:\n" << percentile_value << "\n" << first << "\n" << last << std::endl;
    *value = percentile_value;
    std::pair<long, long> res(first, last);
    return res;
}