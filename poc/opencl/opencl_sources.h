//
// Created by topnax on 23.11.21.
//
#pragma once

#include <string>

const std::string hello_world_code = R"CLC(

__kernel void run(__global ulong* data, __global ulong* outs) {
    int i = get_global_id(0);
    outs[i] = (data[i] * i * 2) >> 4;
}

)CLC";

