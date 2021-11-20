//
// Created by topnax on 20.11.21.
//

#include <fstream>
#include <tbb/flow_graph.h>
#include <tbb/combinable.h>
#include "smp_bucketing.h"
#include "double_utils.h"
#include "bucketing_constants.h"

std::pair<std::vector<uint64_t>, uint64_t> create_buckets_smp(char *file_name) {
    // open input file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare input_node buffer for reading from the file
    std::vector<char> buffer(NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    // define unsigned long vector type
    using buckets = std::vector<uint64_t>;

    // initialize input_node new flow graph
    tbb::flow::graph g;

    // prepare input_node factory for each thread's bucket vector
    tbb::combinable<buckets> priv_h{
            []() {
                return buckets(BUCKET_COUNT);
            }
    };

    // define an input node that reads from input_node file
    tbb::flow::input_node<std::vector<char>> input_node(g, [&](std::vector<char> &out) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        // check whether eof has been reached
        if (fin.gcount() < NUMBER_SIZE_BYTES) {
            return false;
        }

        // send the read data to the next nodes
        out = std::vector<char>(buffer.begin(), buffer.begin() + fin.gcount());

        return true;
    });

    // define input_node multifunction_node type (so that one node can produce multiple items)
    using node_t = tbb::flow::multifunction_node<std::vector<char>, std::tuple<uint64_t>>;

    node_t processing_node(g, tbb::flow::unlimited, [&priv_h](std::vector<char> data, node_t::output_ports_type &p) {
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

// TODO implement SMP file pruning in order to get the actual percentile value
struct bucket_item {
    uint64_t count;
    uint64_t lowest_index;
    uint64_t highest_index;
};

std::pair<double, std::pair<uint64_t, uint64_t>> find_percentile_value_smp(uint64_t bucket, uint64_t percentile_position_in_bucket, char *file_name) {
    // open input file stream
    std::ifstream fin(file_name, std::ifstream::in | std::ifstream::binary);

    // prepare input_node buffer for reading from the file
    std::vector<char> buffer(NUMBER_SIZE_BYTES * NUMBER_BUFFER_SIZE, 0);

    tbb::flow::graph g;

    // define an input node that reads from input_node file
    tbb::flow::input_node<std::vector<char>> input_node(g, [&](std::vector<char> &out) {
        // read from the file stream
        fin.read(buffer.data(), buffer.size());

        // check whether eof has been reached
        if (fin.gcount() < NUMBER_SIZE_BYTES) {
            return false;
        }

        // send the read data to the next nodes
        out = std::vector<char>(buffer.begin(), buffer.begin() + fin.gcount());

        return true;
    });

    // define input_node multifunction_node type (so that one node can produce multiple items)
    using node_t = tbb::flow::multifunction_node<std::vector<char>, std::tuple<uint64_t>>;

    node_t processing_node(g, tbb::flow::unlimited, [&bucket](std::vector<char> data, node_t::output_ports_type &p) {

        // determine how many numbers have been read
        auto numbers_read = data.size() / NUMBER_SIZE_BYTES;

        // iterate over numbers read
        for (int i = 0; i < numbers_read; ++i) {
            // convert data to an unsigned long
            uint64_t content = *((uint64_t *) &data[i * (NUMBER_SIZE_BYTES / sizeof(char))]);

            if (!is_valid_double(content)) continue;

            // calculate the bucket index
            uint64_t bucket_index = content >> NUMBER_SHIFT;

            if (bucket_index == bucket) {
                // TODO this number belongs to the bucket we are analysing
            }
        }
    });

    return std::pair(2.0, std::pair(1,1));
}
