//
// Created by topnax on 23.11.21.
//
#pragma once

#include <string>

const std::string hello_world_code = R"CLC(


#define DOUBLE_FLOAT_EXPONENT_MASK 0x7FF0000000000000
#define DOUBLE_FLOAT_MANTISSA_MASK 0xFFFFFFFFFFFFF

__kernel void run(__global ulong* data, __global ulong* outs, int shift) {
    int i = get_global_id(0);
    ulong data_item = data[i];

    outs[i] = ((data_item * i * 2) >> shift) & DOUBLE_FLOAT_MANTISSA_MASK;
}

)CLC";

