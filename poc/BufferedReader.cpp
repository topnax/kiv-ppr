//
// Created by topnax on 16.11.21.
//

#include <fstream>
#include "BufferedReader.h"

const unsigned long DOUBLE_FLOAT_EXPONENT_MASK = 0x7FF0000000000000;
const unsigned long DOUBLE_FLOAT_MANTISSA_MASK = 0xFFFFFFFFFFFFF;

bool BufferedReader::readNext(uint64_t &out) {
    while (this->buffer_position < this->last_read || readFile()) {
        uint64_t content = *((uint64_t *) &this->buffer[buffer_position * (NUMBER_SIZE_BYTES / sizeof(char))]);
        buffer_position++;

        auto exponent = content & DOUBLE_FLOAT_EXPONENT_MASK;
        if (exponent == 0) {
            if ((content & DOUBLE_FLOAT_MANTISSA_MASK) != 0) {
                // SUBNORMAL
                continue;
            }
        } else if (exponent == DOUBLE_FLOAT_EXPONENT_MASK) {
            // INFINITE / NAN
            continue;
        }
        out = content;
        return true;
    }
    return false;
}

bool BufferedReader::readFile() {
    this->input.read(buffer.data(), buffer.size());
    this->last_read = input.gcount() * sizeof(char) / NUMBER_SIZE_BYTES;
    //printf("read %zu\n", this->buffer_position);
    this->buffer_position = 0;
    this->read_count++;
    return this->last_read > 0;
}
