#include "core/graph.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: classify_edges [path] [memory budget in GB] [weighted]\n");
        exit(-1);
    }

    std::string path = argv[1];
    long memory_bytes = (argc >= 3) ? atol(argv[2]) * 1024L * 1024L * 1024L : 8L * 1024L * 1024L * 1024L;

    Graph graph(path);
    graph.set_memory_bytes(memory_bytes);
    
    double start_time = get_time();

    if (argc < 4) {
        // Counters for edge classifications
        long red_edges = 0, black_edges = 0, crossover_edges = 0;

        // Stream through edges to classify
        graph.stream_edges<VertexId>([&](Edge &e) {
            bool source_red = (e.source & 1) != 0; // Check if source vertex is red
            bool target_red = (e.target & 1) != 0; // Check if target vertex is red

            if (source_red) {
                write_add(&red_edges, long(1));
            } else {
                write_add(&black_edges, long(1));
            }

            // Check if the edge is a crossover
            if (source_red != target_red) {
                write_add(&crossover_edges, long(1));
            }
            return 0;
        });

        // Output results
        printf("Total red edges: %ld\n", red_edges);
        printf("Total black edges: %ld\n", black_edges);
        printf("Total crossover edges: %ld\n", crossover_edges);
        double conductance = double(crossover_edges) / double(std::min(red_edges, black_edges));
        printf("Conductance: %f\n", conductance);
    } else {
        // Counters for edge classifications
        long red_edges = 0, black_edges = 0, crossover_edges = 0;
        int red_weight = 0, black_weight = 0, crossover_weight = 0;

        // Stream through edges to classify
        graph.stream_edges<VertexId>([&](Edge &e) {
            bool source_red = (e.source & 1) != 0; // Check if source vertex is red
            bool target_red = (e.target & 1) != 0; // Check if target vertex is red

            if (source_red) {
                write_add(&red_edges, long(1));
                write_add(&red_weight, int(e.weight));
            } else {
                write_add(&black_edges, long(1));
                write_add(&black_weight, int(e.weight));
            }

            // Check if the edge is a crossover
            if (source_red != target_red) {
                write_add(&crossover_edges, long(1));
                write_add(&crossover_weight, int(e.weight));
            }
            return 0;
        });

        // Output results
        printf("Total red edges: %ld, total red weight: %d\n", red_edges, red_weight);
        printf("Total black edges: %ld, total black weight: %d\n", black_edges, black_weight);
        printf("Total crossover edges: %ld, total crossover weight: %d\n", crossover_edges, crossover_weight);
        double conductance = crossover_edges / double(std::min(red_edges, black_edges));
        double conductance_weight = crossover_weight / double(std::min(red_weight, black_weight));
        printf("Conductance: %f, Conductance (weighted): %f\n", conductance, conductance_weight);
    }

    double end_time = get_time();
    printf("Time = %.2f seconds\n", end_time - start_time);

    return 0;
}
