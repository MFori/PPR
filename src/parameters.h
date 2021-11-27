/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_PARAMETERS_H
#define PPR_PARAMETERS_H

#include "percentile_finder.h"

/**
 * Program parameters holder
 */
struct ProgramParams {
    /**
     * Name or path to input file
     */
    char *file_name;
    /**
     * Name of OpenCl file (can be nullptr if processor != OpenCl)
     */
    char *cl_device;
    /**
     * Percentile number 1-100
     */
    int percentile;
    /**
     * Processing type
     */
    ProcessorType processor;
};

/**
 * Parse program params
 * @param argc number of params
 * @param argv params
 * @param params filled with parsed params
 * @return 0 if params are valid <0> otherwise
 */
int parseParams(int argc, char *argv[], ProgramParams *params);

#endif /* PPR_PARAMETERS_H */
