
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <unordered_map>

struct Edge {
    int to;
    int weight;
};

class Graph {
private:
    std::vector<std::vector<Edge>> adj;
    std::unordered_map<int, int> orig_to_internal;
    std::vector<int> internal_to_orig;
    int n_nodes;
    long long n_edges;

public:
    Graph();

    int get_or_create(int original_id);
    int find_internal(int original_id) const;
    int get_original(int internal_id) const;

    void add_undirected_edge(int u_internal, int v_internal, int weight);

    const std::vector<Edge>& neighbors(int u_internal) const;

    int num_nodes() const;
    long long num_edges() const;

    void reserve_nodes(int n);
};

#endif
