//
// Created by topnax on 20.11.21.
//

#ifndef POC_DOUBLE_UTILS_H
#define POC_DOUBLE_UTILS_H


const unsigned long DOUBLE_FLOAT_EXPONENT_MASK = 0x7FF0000000000000;
const unsigned long DOUBLE_FLOAT_MANTISSA_MASK = 0xFFFFFFFFFFFFF;

inline bool is_valid_double(uint64_t num) {
    // filter out SUBNORMAL, INFINITE or NAN numbers
    auto exponent = num & DOUBLE_FLOAT_EXPONENT_MASK;
    if (exponent == 0) {
        if ((num & DOUBLE_FLOAT_MANTISSA_MASK) != 0) {
            // SUBNORMAL
            return false;
        }
    } else if (exponent == DOUBLE_FLOAT_EXPONENT_MASK) {
        // INFINITE / NAN
        return false;
    }
    return true;
}

#endif //POC_DOUBLE_UTILS_H
