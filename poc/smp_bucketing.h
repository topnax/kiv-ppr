//
// Created by topnax on 20.11.21.
//
#include <vector>
#include <cstdint>

#ifndef POC_SMP_BUCKETING_H
#define POC_SMP_BUCKETING_H

using buckets_and_item_count = std::pair<std::vector<uint64_t>, uint64_t>;

buckets_and_item_count create_buckets_smp(char *file_name);

#endif //POC_SMP_BUCKETING_H
