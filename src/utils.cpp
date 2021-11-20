/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */

#include <cmath>
#include "utils.h"

bool utils::is_valid_double(double d) {
    int cl = std::fpclassify(d);
    return cl == FP_NORMAL || cl == FP_ZERO;
}

size_t utils::get_file_size(std::ifstream *file) {
    file->seekg(0, std::ios::end);
    return file->tellg();
}
