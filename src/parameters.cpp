/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <cstdlib>
#include <fstream>
#include "parameters.h"

int parseParams(int argc, char *argv[], ProgramParams *params) {
    if (argc != 4) {
        return -1;
    }

    std::string file_name = argv[1];
    std::ifstream file(file_name, std::ios::binary);

    bool file_good = file.good();
    file.close();
    if (!file_good) {
        return -1;
    }
    params->file_name = argv[1];

    int percentile = atoi(argv[2]);
    if (!percentile && strcmp(argv[2], "0") != 0) {
        return -1;
    }
    if (percentile < 1 || percentile > 100) {
        return -1;
    }
    params->percentile = percentile;

    if (strcmp(argv[3], "single") == 0) {
        params->processor = ProcessorType::Single;
    } else if (strcmp(argv[3], "SMP") == 0) {
        params->processor = ProcessorType::SMP;
    } else {
        // TODO check if OpenCL device available
    }

    return 0;
}