
#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include "graph.hpp"
#include <vector>
#include <climits>

struct DijkstraResult {
    std::vector<long long> dist;
    std::vector<int> parent;
    long long nodes_explored;
};

const long long INF_DIST = LLONG_MAX;

DijkstraResult dijkstra(const Graph& g, int source_internal, int target_internal = -1);

std::vector<int> reconstruct_path(const std::vector<int>& parent, int source, int target);

#endif
