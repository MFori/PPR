/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <cstdlib>
#include <fstream>
#include "parameters.h"
#include "logging.h"
#include "utils.h"

int parseParams(int argc, char *argv[], ProgramParams *params) {
    LOG_D("Parsing program params...");
    // check params count
    if (argc != 4) {
        return -1;
    }

    std::string file_name = argv[1];
    std::ifstream file(file_name, std::ios::binary);

    // check if file exists and can be read
    bool file_good = file.good();
    file.close();
    if (!file_good) {
        LOG("Cannot open file " << file_name);
        return -1;
    }
    params->file_name = argv[1];

    // parse percentile number
    int percentile = atoi(argv[2]);
    if (!percentile && strcmp(argv[2], "0") != 0) {
        return -1;
    }
    // check percentile number bound
    if (percentile < 1 || percentile > 100) {
        return -1;
    }
    params->percentile = percentile;

    // get processor type
    if (strcmp(argv[3], "single") == 0) {
        params->processor = ProcessorType::Single;
    } else if (strcmp(argv[3], "SMP") == 0) {
        params->processor = ProcessorType::SMP;
    } else {
        params->processor = ProcessorType::OpenCL;
        // check if OpenCl device with name is available
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

    // params are valid
    return 0;
}