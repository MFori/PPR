/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "parameters.h"

void help() {
    std::cout << "Invalid program params, usage:\n";
    std::cout << "pprsolver.exe file percentile processor\n";
    std::cout << "\t file - file path\n";
    std::cout << "\t percentile - number 1 - 100\n";
    std::cout << "\t processor - \"single\" / \"SMP\" / OpenCL device name\n";
}

int main(int argc, char *argv[]) {
    ProgramParams params{};
    if (parseParams(argc, argv, params)) {
        help();
        return 0;
    }

    std::cout << "Hello World!\n";
}