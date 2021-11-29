//
// Created by topnax on 20.11.21.
//

#include <fstream>
#include <tbb/flow_graph.h>
#include <tbb/combinable.h>
#include <iostream>
#include <atomic>
#include <map>
#include "smp_bucketing.h"
#include "double_utils.h"
#include "bucketing_constants.h"
#include "tbb/concurrent_hash_map.h"
#include "bucketing_utils.h"
#include "watchdog.h"


std::pair<std::vector<uint64_t>, uint64_t> create_buckets_smp(char *file_name) {
    // open input file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare input_node buffer for reading from the file
    std::vector<char> buffer(NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    // define unsigned long vector type
    using buckets = std::vector<uint64_t>;

    // initialize input_node new flow graph
    tbb::flow::graph g;

    // TODO might utilize std::vector<atomic<uint64_t>> to reduce memory consumption in exchange for increased CPU time
    // prepare input_node factory for each thread's bucket vector
    tbb::combinable<buckets> priv_h{
            []() {
                return buckets(BUCKET_COUNT);
            }
    };

    // define an input node that reads from input_node file
    tbb::flow::input_node<std::vector<char>> input_node(g, [&](tbb::flow_control& fc) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        // check whether eof has been reached
        if (fin.gcount() < NUMBER_SIZE_BYTES) {
            fc.stop();
            return std::vector<char>();
        }

        // send the read data to the next nodes
        return std::vector<char>(buffer.begin(), buffer.begin() + fin.gcount());

        // return true;
     });

    // define input_node multifunction_node type (so that one node can produce multiple items)
    using node_t = tbb::flow::multifunction_node<std::vector<char>, std::tuple<uint64_t>>;

    node_t processing_node(g, tbb::flow::unlimited, [&priv_h](std::vector<char> data, node_t::output_ports_type &p) {
        ThreadWatchdog::kick();
        // get buckets_vec local to this thread
        buckets &local_buckets = priv_h.local();

        // determine how many numbers have been read
        auto numbers_read = data.size() / NUMBER_SIZE_BYTES;

        // iterate over numbers read
        for (int i = 0; i < numbers_read; ++i) {
            // convert data to an unsigned long
            uint64_t content = *((uint64_t *) &data[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

            if (!is_valid_double(content)) continue;

            // calculate the bucket index
            unsigned long bucket_index = content >> NUMBER_SHIFT;

            // update the bucket count
            local_buckets[bucket_index] += 1;
        }
    });

    // connect first node with the second one
    tbb::flow::make_edge(input_node, processing_node);

    // activate the input node
    input_node.activate();

    // wait for all nodes to finish
    g.wait_for_all();

    // prepare the final buckets vector
    buckets buckets_vec(BUCKET_COUNT);

    // combine each thread's local buckets vector into a single one
    priv_h.combine_each([&](buckets a) {
        std::transform(
                buckets_vec.begin(),
                buckets_vec.end(),
                a.begin(),
                buckets_vec.begin(),
                std::plus<>()
        );
    });

    // compute the number of items present across all buckets
    uint64_t total_bucket_item_count = 0;
    for (auto i = 0; i < BUCKET_COUNT; i++) {
        total_bucket_item_count += buckets_vec[i];
    }

    return std::pair(buckets_vec, total_bucket_item_count);
}

std::pair<double, std::pair<uint64_t, uint64_t>>
find_percentile_value_smp(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name) {
    // open input file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare input_node buffer for reading from the file
    std::vector<char> buffer(NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    // make an alias for the tbb::concurrent_hash_map map with our types
    using occurences_map = tbb::concurrent_hash_map<double, std::unique_ptr<bucket_item>>;
    occurences_map occurences;

    // initialize TBB flow graph
    tbb::flow::graph g;

    // define input type of a function node that will process data from the input node
    using node_t = tbb::flow::function_node<std::pair<std::vector<char>, uint64_t>, bool>;

    // remember the current index while reading the file
    uint64_t index = 0;

    // define an input node that reads from the input file
    tbb::flow::input_node<node_t::input_type> input_node(g, [&index, &fin, &buffer](tbb::flow_control& fc) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        // check whether eof has been reached
        if (fin.gcount() < NUMBER_SIZE_BYTES) {
            fc.stop();
            return std::pair(std::vector<char>(), index);
        }

        // send the read data to the next nodes
        auto out = std::pair(
                std::vector<char>(buffer.begin(), buffer.begin() + fin.gcount()),
                index
        );

        // increment the current index in the file
        index += fin.gcount();

        return out;
    });

    // define a prescription for nodes that will process incoming data from the input node
    node_t processing_node(g, tbb::flow::unlimited,
                           [&bucket, &occurences](node_t::input_type inp) {
                               ThreadWatchdog::kick();
                               // TODO refactor into a function
                               // gather the data to be processed
                               auto data = inp.first;

                               auto index = inp.second;

                               // determine how many numbers have been read
                               auto numbers_read = data.size() / NUMBER_SIZE_BYTES;

                               // iterate over numbers read
                               for (int i = 0; i < numbers_read; ++i) {
                                   // convert data to an unsigned long
                                   uint64_t content = *((uint64_t *) &data[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

                                   if (is_valid_double(content)) {
                                       // calculate the bucket index
                                       uint64_t bucket_index = content >> NUMBER_SHIFT;

                                       if (bucket_index == bucket) {
                                           // get the double representation
                                           auto number = *((double *) &content);

                                           // non-const map accessor that allows for thread-safe update (till it goes out of scope)
                                           auto accessor = occurences_map::accessor();

                                           // attempt to insert the number
                                           bool new_inserted = occurences.insert(accessor, number);

                                           if (new_inserted) {
                                               // this key has not been inserted before, initialize a new item
                                               auto *item = new bucket_item;
                                               item->count = 1;
                                               item->lowest_index = index;
                                               item->highest_index = index;

                                               // write changes to the accessor
                                               accessor->second = std::unique_ptr<bucket_item>(item);
                                           } else {
                                               // this key already exists in the map,
                                               // get the item from the accessor
                                               auto item = accessor->second.get();

                                               // increment the item count
                                               item->count = item->count + 1;

                                               // update indices
                                               if (item->lowest_index > index) {
                                                   item->lowest_index = index;
                                               }
                                               if (item->highest_index < index) {
                                                   item->highest_index = index;
                                               }
                                           }
                                       }
                                   }
                                   index += NUMBER_SIZE_BYTES;
                               }
                               return true;
                           });

    // connect the input node with the processing node(s)
    tbb::flow::make_edge(input_node, processing_node);

    // activate the input node
    input_node.activate();

    // wait for all nodes to finish
    g.wait_for_all();

    // prepare for summing the occurrences in order to find the percentile value
    uint64_t sum = 0;
    auto result_key = occurences.begin()->first;
    auto result_item = occurences.begin()->second.get();

    // the occurrences map has to be sorted by key
    std::vector<std::pair<double, bucket_item *>> occurrences_sorted(occurences.size());

    // copy the map contents into a vector
    // TODO memory consumption might be improved
    auto i = 0;
    for (const auto &v : occurences) {
        occurrences_sorted[i] = std::pair(v.first, v.second.get());
        i++;
    }

    // sort the occurrences
    std::sort(occurrences_sorted.begin(), occurrences_sorted.end());

    // find the percentile
    for (auto const[key, val] : occurrences_sorted) {
        sum += val->count;
        if (sum > percentile_position_in_bucket) {
            result_key = key;
            result_item = val;
            break;
        }
    }

    // return the result
    return std::pair(result_key, std::pair(result_item->lowest_index, result_item->highest_index));
}
