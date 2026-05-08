
#include "graph.hpp"

Graph::Graph() : n_nodes(0), n_edges(0) {}

int Graph::get_or_create(int original_id) {
    auto it = orig_to_internal.find(original_id);
    if (it != orig_to_internal.end()) return it->second;

    int new_idx = n_nodes++;
    orig_to_internal[original_id] = new_idx;
    internal_to_orig.push_back(original_id);
    adj.emplace_back();
    return new_idx;
}

int Graph::find_internal(int original_id) const {
    auto it = orig_to_internal.find(original_id);
    return (it != orig_to_internal.end()) ? it->second : -1;
}

int Graph::get_original(int internal_id) const {
    if (internal_id < 0 || internal_id >= (int)internal_to_orig.size()) return -1;
    return internal_to_orig[internal_id];
}

void Graph::add_undirected_edge(int u, int v, int weight) {
    adj[u].push_back({v, weight});
    adj[v].push_back({u, weight});
    n_edges++;
}

const std::vector<Edge>& Graph::neighbors(int u_internal) const {
    return adj[u_internal];
}

int Graph::num_nodes() const { return n_nodes; }
long long Graph::num_edges() const { return n_edges; }

void Graph::reserve_nodes(int n) {
    adj.reserve(n);
    internal_to_orig.reserve(n);
    orig_to_internal.reserve(n);
}
