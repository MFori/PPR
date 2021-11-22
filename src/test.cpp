/**
 * Created by Martin on 21.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include <iostream>
#include "test.h"
#include "naive.h"
#include "percentile_finder.h"

void test_1(char *file_name) {
    for (int i = 1; i <= 100; i++) {
        double naive;
        auto limits = find_percentile_naive(file_name, i, &naive);

        Result result{};
        run(file_name, i, ProcessorType::Single, nullptr, &result);

        std::cout << "test (" << i << "), naive: " << naive << ", run: " << result.value
         << "result: " << (naive == result.value ? "OK" : "FAILED") << std::endl;
    }
}