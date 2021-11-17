/**
 * Created by Martin on 17.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_PARAMETERS_H
#define PPR_PARAMETERS_H

enum class ProcessorType {
    Single,
    SMP,
    OpenCL
};

struct ProgramParams {
    char *file_name;
    int percentile;
    ProcessorType processor;
};

int parseParams(int argc, char *argv[], ProgramParams &params);

#endif /* PPR_PARAMETERS_H */