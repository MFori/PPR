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
        return 0;
    }

    LOG_D("Params parsed:\n- file: " << params.file_name << "\n- percentile: " << params.percentile
              << "\n- processor type: " << (int) params.processor);

    //test_1(params.file_name);
    //return 0;

    auto state = State();
    // TODO init watchdog
    struct Result result{};
    run(params.file_name, params.percentile, params.processor, &state, &result);

    print_result(&result);

    LOG_D("Program end.");

    return EXIT_SUCCESS;
}