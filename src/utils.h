/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_UTILS_H
#define PPR_UTILS_H

#include <fstream>

namespace utils {

    bool is_valid_double(double d);

    size_t get_file_size(std::ifstream *file);
}

#endif /* PPR_UTILS_H */
