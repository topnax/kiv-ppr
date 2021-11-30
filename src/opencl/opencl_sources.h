//
// Created by topnax on 23.11.21.
//
#pragma once

#include <string>

const std::string create_buckets_source = R"CLC(

#define DOUBLE_FLOAT_EXPONENT_MASK 0x7FF0000000000000
#define DOUBLE_FLOAT_MANTISSA_MASK 0xFFFFFFFFFFFFF

__kernel void create_buckets(__global ulong* data, __global ulong* outs, int shift) {
    int i = get_global_id(0);
    ulong data_item = data[i];
    ulong exponent = data_item & DOUBLE_FLOAT_EXPONENT_MASK;
    if (exponent == 0) {
        if ((data_item & DOUBLE_FLOAT_MANTISSA_MASK) != 0) {
            // SUBNORMAL
            return;
        }
    } else if (exponent == DOUBLE_FLOAT_EXPONENT_MASK) {
        // INFINITE / NAN
        return;
    }

    atom_inc(&outs[((data_item) >> shift) & DOUBLE_FLOAT_MANTISSA_MASK]);
}

)CLC";

