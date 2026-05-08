
#ifndef SUBGRAPH_HPP
#define SUBGRAPH_HPP

#include "graph.hpp"
#include <vector>
#include <string>

struct SubgraphEdge {
    int u_internal;
    int v_internal;
    int weight;
};

struct Subgraph {
    std::vector<int> nodes_internal;
    std::vector<SubgraphEdge> edges;
};

struct MSTResult {
    long long total_weight;
    int num_edges;
};

struct DAGResult {
    bool is_dag;
    int back_edges;
    int tree_edges;
};

Subgraph build_induced_subgraph(const Graph& g, const std::vector<int>& path_nodes_internal);

MSTResult kruskal_mst(const Subgraph& sg);

DAGResult check_dag_directed(const Subgraph& sg);

void save_subgraph(const Subgraph& sg, const Graph& g, const std::string& filename);

void save_subgraph_analysis(const Subgraph& sg, const MSTResult& mst,
                             const DAGResult& dag, const std::string& filename);

#endif
