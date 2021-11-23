/**
 * Created by Martin on 23.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_SMP_FILE_READER_H
#define PPR_SMP_FILE_READER_H

#include <fstream>
#include <tbb/flow_graph.h>

class SMPFileReader {
public:
    SMPFileReader(std::ifstream *file) : file(file) {};

    SMPFileReader(const SMPFileReader &a) : file(a.file) {};

    ~SMPFileReader() {};

    double i = 0;

    double operator()(tbb::flow_control &fc) {
        if (i > 10) {
            fc.stop();
        }
        return i++;
    }

private:
    std::ifstream *file;
};

#endif /* PPR_SMP_FILE_READER_H */
