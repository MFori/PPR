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

void help() {
    std::cout << "Invalid program params, usage:" << std::endl;
    std::cout << "pprsolver.exe file percentile processor" << std::endl;
    std::cout << "\t file - file path" << std::endl;
    std::cout << "\t percentile - number 1 - 100" << std::endl;
    std::cout << "\t processor - \"single\" / \"SMP\" / OpenCL device name" << std::endl;
}

void print_result(Result *result) {
    std::cout << "Result found:" << std::endl;
    std::cout << "- Percentile value: " << result->value << " (" << std::hexfloat << std::uppercase << result->value << ")" << std::endl;
    std::cout << "- First position: " << std::hex << std::uppercase << result->first_pos << std::endl;
    std::cout << "- Last position: " << std::hex << std::uppercase << result->last_pos << std::endl;
}

int main(int argc, char *argv[]) {
    std::cout << "Starting..." << std::endl;
    struct ProgramParams params{};
    if (parseParams(argc, argv, &params)) {
        help();
        return 0;
    }

    std::cout << "Params parsed:\n- file: " << params.file_name << "\n- percentile: " << params.percentile
              << "\n- processor type: " << (int) params.processor << std::endl;

    //test_1(params.file_name);
    //return 0;

    auto state = State();
    // TODO init watchdog
    struct Result result{};
    run(params.file_name, params.percentile, params.processor, &state, &result);

    print_result(&result);

    std::cout << "Program end." << std::endl;

    return EXIT_SUCCESS;
}