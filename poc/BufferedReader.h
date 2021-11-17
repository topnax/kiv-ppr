//
// Created by topnax on 16.11.21.
//

#include <iostream>
#include <vector>
#include "bucketing.h"

#ifndef POC_BUFFEREDREADER_H
#define POC_BUFFEREDREADER_H


class BufferedReader {

public:
    BufferedReader(std::ifstream &input, size_t size) : input(input), size(size), buffer(std::vector<char>(NUMBER_SIZE_BYTES * size)) {};
    std::ifstream &input;
    size_t size;
    std::vector<char> buffer;

    bool eof = false;
    size_t buffer_position = 0;
    size_t read_count = 0;
    size_t last_read = 0;
    bool first = true;

    bool readNext(uint64_t &out);

    bool readFile();
private:

};


#endif //POC_BUFFEREDREADER_H
