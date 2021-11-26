/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "parameters.h"
#include "logging.h"
#include "utils.h"

int parseParams(int argc, char *argv[], ProgramParams *params) {
    std::cout << "Parsing program params...\n";
    if (argc != 4) {
        return -1;
    }

    std::string file_name = argv[1];
    std::ifstream file(file_name, std::ios::binary);

    bool file_good = file.good();
    file.close();
    if (!file_good) {
        LOG("Cannot open file " << file_name);
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
        params->processor = ProcessorType::OpenCL;
        if (!utils::has_cl_device(argv[3])) {
            LOG("OpenCL device \"" << argv[3] << "\" not found. Available devices: ");
            auto devices = utils::get_cl_devices();
            for (const auto &device : devices) {
                LOG(" - " << device);
            }
            return -1;
        } else {
            params->cl_device = argv[3];
        }
    }

    return 0;
}