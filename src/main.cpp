/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "parameters.h"
#include "buckets.h"
#include "logging.h"
#include "watchdog.h"

/**
 * Watchdog timeout in seconds
 */
const unsigned int WATCHDOG_TIMEOUT_S = 10;

/**
 * Print help about program parameters
 */
void help() {
    LOG("Invalid program params, usage:");
    LOG("pprsolver.exe file percentile processor");
    LOG("\t file - file path");
    LOG("\t percentile - number 1 - 100");
    LOG("\t processor - \"single\" / \"SMP\" / OpenCL device name");
}

/**
 * Print result (value and positions as hex numbers)
 * @param result
 */
void print_result(Result *result) {
    std::wcout << std::hexfloat << result->value << std::hex
               << " " <<
               result->first_pos
               << " " <<
               result->last_pos
               << std::endl;

    LOG_D("Result found:");
    LOG_D("- Percentile value: " << result->value << " (" << std::hexfloat << std::uppercase << result->value << ")");
    LOG_D("- First position: " << std::hex << std::uppercase << result->first_pos);
    LOG_D("- Last position: " << std::hex << std::uppercase << result->last_pos);
}

// entry point
int main(int argc, char *argv[]) {
    LOG_D("Starting...");

    // parse params
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
    //create_test_file("test_4.test");
    //return 0;

    // start watchdog
    Watchdog::start(WATCHDOG_TIMEOUT_S, [] {
        LOG("Watchdog bite. Killing app...");
        exit(EXIT_FAILURE);
    });

    // run percentile finder
    struct Result result{};
    run(params.file_name, params.percentile, params.processor, params.cl_device, &result);

    Watchdog::stop();
    print_result(&result);
    LOG_D("Program end.");

    return EXIT_SUCCESS;
}