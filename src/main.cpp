/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "parameters.h"
#include "buckets.h"
#include "test.h"
#include "logging.h"
#include "watchdog.h"

const unsigned int WATCHDOG_TIMEOUT_S = 10;

void help() {
    LOG("Invalid program params, usage:");
    LOG("pprsolver.exe file percentile processor");
    LOG("\t file - file path");
    LOG("\t percentile - number 1 - 100");
    LOG("\t processor - \"single\" / \"SMP\" / OpenCL device name");
}

void print_result(Result *result) {
    LOG("Result found:");
    LOG("- Percentile value: " << result->value << " (" << std::hexfloat << std::uppercase << result->value << ")");
    LOG("- First position: " << std::hex << std::uppercase << result->first_pos);
    LOG("- Last position: " << std::hex << std::uppercase << result->last_pos);
}

int main(int argc, char *argv[]) {
    LOG_D("Starting...");
    struct ProgramParams params{};
    if (parseParams(argc, argv, &params)) {
        help();
        return EXIT_SUCCESS;
    }

    LOG_D("Params parsed:\n- file: "
                  << params.file_name
                  << "\n- percentile: " << params.percentile
                  << "\n- processor type: " << (int) params.processor
                  << "\n- OpenCl device: " << (params.processor == ProcessorType::OpenCL ? params.cl_device : "-"));

    //test_1(params.file_name);
    //return 0;

    Watchdog::start(WATCHDOG_TIMEOUT_S, [] {
        LOG("Watchdog bite. Killing app...");
        exit(EXIT_FAILURE);
    });

    struct Result result{};
    run(params.file_name, params.percentile, params.processor, &result);

    Watchdog::stop();

    print_result(&result);

    LOG_D("Program end.");

    return EXIT_SUCCESS;
}