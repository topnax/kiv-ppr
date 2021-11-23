//
// Created by topnax on 20.11.21.
//
#pragma once

#include <vector>
#include <cstdint>
#include <cmath>

// size of the number buffer when reading from a file
const int NUMBER_BUFFER_SIZE = 1000;

// size of numbers being loaded from a file in bytes
const int NUMBER_SIZE_BYTES = sizeof(double);

// size of numbers being loaded from a file in bits
const int NUMBER_SIZE_BITS = NUMBER_SIZE_BYTES * 8;

// max number possible
const uint64_t NUMBER_MAX = 0xFFFFFFFFFFFFFFFFu;

// number of bits kept when calculating a bucket index
const int BUCKET_BITS = 20;

// number of bits to shift the loaded number by
const int NUMBER_SHIFT = NUMBER_SIZE_BITS - BUCKET_BITS;

// bucket index length
const int BUCKET_KEY_LENGTH_BITS = NUMBER_SIZE_BITS - NUMBER_SHIFT;

// calculate the number of buckets
const unsigned long BUCKET_COUNT = pow(2, BUCKET_KEY_LENGTH_BITS);

// number of bits to be taken from mantissa when performing subbucketing
const unsigned long SUB_BUCKET_MANTISSA_BITS = 20;

// number of sub buckets
const unsigned long SUB_BUCKET_COUNT = pow(2, SUB_BUCKET_MANTISSA_BITS);

// subbucket mantissa bit mask :)
const unsigned long SUB_BUCKET_MANTISSA_MASK =
        ((NUMBER_MAX >> (NUMBER_SIZE_BITS - (BUCKET_BITS + SUB_BUCKET_MANTISSA_BITS)))
                << (NUMBER_SIZE_BITS - SUB_BUCKET_MANTISSA_BITS)) >> BUCKET_BITS;

// number of bits to shift the loaded number by when calculating subbucket index
const unsigned long SUB_BUCKET_SHIFT = NUMBER_SIZE_BITS - (SUB_BUCKET_MANTISSA_BITS + BUCKET_BITS);


// CONSTRAINTS:
const unsigned long MEBI_BYTE = pow(pow(2, 10), 2);
const unsigned long MEMORY_LIMIT = MEBI_BYTE * 250;

// TODO incorrect bucket count
const unsigned long MAX_BUCKET_COUNT = (unsigned long) ((double) MEMORY_LIMIT * 0.8);
