#include "core/graph.hpp"
#include <fstream>

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: kcore [path] [k] [memory budget in GB] [output file]\n");
        exit(-1);
    }

    std::string path = argv[1];
    int k = atoi(argv[2]);
    int mem = (argc >= 4) ? atol(argv[3]) : -1;
    long memory_bytes = ((argc >= 4) && mem != 0) ? mem * 1024L * 1024L * 1024L : 8L * 1024L * 1024L * 1024L;

    Graph graph(path);
    graph.set_memory_bytes(memory_bytes);

    Bitmap * active_in = graph.alloc_bitmap();
    Bitmap * active_out = graph.alloc_bitmap();
    Bitmap * need_delete = graph.alloc_bitmap();
    BigVector<int> degree(graph.path + "/degree", graph.vertices); // Store node degrees

    active_out->fill();
    degree.fill(0);

    // Step 1: Initialize node degrees
    graph.stream_edges<int>([&](Edge &e) {
        write_add(&degree[e.source], 1);
        write_add(&degree[e.target], 1);
        return 0;
    });

    // Step 2: Iteratively remove nodes with degree < k
    bool updated;
    int iter = 0;
    double start_time = get_time();
    int remained = graph.vertices;

    do {
        updated = false;
        std::swap(active_in, active_out);
        active_out->clear();
        need_delete->clear();
        graph.hint(degree);
        int del = graph.stream_vertices<int>([&](VertexId i) {
            if (degree[i] < k) {
                need_delete->set_bit(i);
                updated = true;
                return 1;
            } else {
                active_out->set_bit(i);
                return 0;
            }
        }, active_in);
        printf("Deleted %d nodes in iteration %d\n", del, iter++);
        remained -= del;
        printf("Remained %d nodes\n", remained);

        graph.stream_edges<int>([&](Edge &e) {
            if (need_delete->get_bit(e.source) && active_in->get_bit(e.target)) {
                write_add(&degree[e.target], -1);
            }
            if (need_delete->get_bit(e.target)) {
                write_add(&degree[e.source], -1);
            }
            return 0;
        }, active_in);
    } while (updated);

    // Output node degrees
    if (argc >= 5 || mem == 0) {
        std::ofstream myfile;
        if (!file_exists("output")) {
            create_directory("output");
        }
        std::string output_file = "output/" + std::to_string(k) + "_core_" + (mem == 0 ? argv[3] :argv[4]) + ".txt";\
        printf("Output file: %s\n", output_file.c_str());
        myfile.open(output_file);
        for (int i = 0; i < graph.vertices; i++) {
            if (active_out->get_bit(i)) {
                myfile << i << " " << degree[i] << std::endl;
            }
        }
    }

    double end_time = get_time();

    // Output remaining K-core nodes
    int num = graph.stream_vertices<int>([&](VertexId i) {
        if (active_out->get_bit(i)) {
            // printf("Node %d is in the %d-core\n", i, k);
            return 1;
        }
        return 0;
    });
    printf("Found %d nodes in the %d-core in %.2f seconds.\n", num, k, end_time - start_time);

    return 0;
}
