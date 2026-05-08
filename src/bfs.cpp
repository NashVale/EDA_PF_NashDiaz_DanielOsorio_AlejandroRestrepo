
#include "bfs.hpp"
#include <queue>

BFSResult bfs(const Graph& g, int source, int target) {
    int n = g.num_nodes();
    BFSResult r;
    r.dist.assign(n, INT_MAX);
    r.parent.assign(n, -1);
    r.nodes_explored = 0;

    if (source < 0 || source >= n) return r;

    std::queue<int> q;
    q.push(source);
    r.dist[source] = 0;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        r.nodes_explored++;

        if (u == target) return r;

        for (const Edge& e : g.neighbors(u)) {
            if (r.dist[e.to] == INT_MAX) {
                r.dist[e.to] = r.dist[u] + 1;
                r.parent[e.to] = u;
                q.push(e.to);
            }
        }
    }
    return r;
}

int bfs_eccentricity(const Graph& g, int source, int& farthest_node) {
    BFSResult r = bfs(g, source, -1);
    int max_d = 0;
    farthest_node = source;
    for (int i = 0; i < (int)r.dist.size(); i++) {
        if (r.dist[i] != INT_MAX && r.dist[i] > max_d) {
            max_d = r.dist[i];
            farthest_node = i;
        }
    }
    return max_d;
}
