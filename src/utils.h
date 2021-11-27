/**
 * Created by Martin on 18.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_UTILS_H
#define PPR_UTILS_H

#include <fstream>
#include <vector>
#include "cl_lib.h"

namespace utils {

    /**
     * Check if double is valid double (not infinity/nan)
     * @param d double to validate
     * @return valid?
     */
    bool is_valid_double(double d);

    /**
     * Get size of file (in bytes)
     * @param file file input stream
     * @return size of file
     */
    size_t get_file_size(std::ifstream *file);

    /**
     * Check if OpenCl device with name exists
     * @param name OpenCl device name
     * @return exists?
     */
    bool has_cl_device(char *name);

    /**
     * Get OpenCl device by name
     * @param name OpenCl device name
     * @return device
     * @throw runtime_error if device with name not exists
     */
    cl::Device get_cl_device(char *name);

    /**
     * Get list of available OpenCl devices names
     * @return list of OpenCl devices names
     */
    std::vector<std::string> get_cl_devices();
}

#endif /* PPR_UTILS_H */
