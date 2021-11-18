/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "parameters.h"
#include "naive.h"
#include "buckets.h"

void help() {
    std::cout << "Invalid program params, usage:\n";
    std::cout << "pprsolver.exe file percentile processor\n";
    std::cout << "\t file - file path\n";
    std::cout << "\t percentile - number 1 - 100\n";
    std::cout << "\t processor - \"single\" / \"SMP\" / OpenCL device name\n";
}

int main(int argc, char *argv[]) {
    struct ProgramParams params{};
    if (parseParams(argc, argv, &params)) {
        help();
        return 0;
    }

    std::ifstream file(params.file_name, std::ifstream::in | std::ifstream::binary);
    size_t total_values = 0;

    auto limits = find_histogram_limits(&file, &total_values);
    std::cout << "limits: " << limits.first << " " << limits.second << "\nread: " << total_values << "\n";
    file.close();

    //find_percentile_naive(params.file_name, params.percentile);

    std::cout << "Hello World!\n";
}