
#ifndef BFS_HPP
#define BFS_HPP

#include "graph.hpp"
#include <vector>
#include <climits>

struct BFSResult {
    std::vector<int> dist;
    std::vector<int> parent;
    long long nodes_explored;
};

BFSResult bfs(const Graph& g, int source_internal, int target_internal = -1);

int bfs_eccentricity(const Graph& g, int source_internal, int& farthest_node);

#endif
