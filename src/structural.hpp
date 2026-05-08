
#ifndef STRUCTURAL_HPP
#define STRUCTURAL_HPP

#include "graph.hpp"
#include <string>

struct GraphStats {
    int num_nodes;
    long long num_edges;
    int main_component_size;
    int num_components;
    double avg_degree;
    int max_degree;
    int max_degree_node_orig;
    int diameter_estimate;
};

GraphStats analyze_graph(const Graph& g);

void save_structural_analysis(const GraphStats& s, const std::string& filename);

#endif
