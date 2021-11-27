/**
 * Created by Martin on 21.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "test.h"
#include "naive.h"
#include "percentile_finder.h"
#include "logging.h"

void test_1(char *file_name) {
    for (int i = 1; i <= 100; i++) {
        double naive;
        double single;
        double smp;
        double opencl;

        auto naive_pos = find_percentile_naive(file_name, i, &naive);

        Result result_single{};
        run(file_name, i, ProcessorType::Single, nullptr, &result_single);
        single = result_single.value;

        Result result_smp{};
        run(file_name, i, ProcessorType::SMP, nullptr, &result_smp);
        smp = result_smp.value;

        Result result_opencl{};
        std::string device_name = "Intel(R) HD Graphics 620";
        run(file_name, i, ProcessorType::OpenCL, (char*) device_name.c_str(), &result_opencl);
        opencl = result_opencl.value;

        bool ok = (naive == single && single == smp && smp == opencl);

        LOG("test (" << i << "): " << (ok ? "OK" : "FAILED"));
        LOG(" - naive: " << naive << " - " << naive_pos.first << " - " << naive_pos.second);
        LOG(" - single: " << single << " - " << result_single.first_pos << " - " << result_single.last_pos);
        LOG(" - smp: " << smp << " - " << result_smp.first_pos << " - " << result_smp.last_pos);
        LOG(" - opencl: " << opencl << " - " << result_opencl.first_pos << " - " << result_opencl.last_pos);
    }
}

void create_test_file(char *file_name) {
    std::ofstream file(file_name, std::ifstream::out | std::ifstream::binary);
    for (double i = 0.0; i <= 1.0;) {
        file.write((char *) &i, sizeof(double));
        i += 0.01;
    }
    file.close();
}