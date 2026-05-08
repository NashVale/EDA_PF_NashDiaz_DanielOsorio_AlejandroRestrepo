
#include "dijkstra.hpp"
#include <queue>
#include <algorithm>

DijkstraResult dijkstra(const Graph& g, int source, int target) {
    int n = g.num_nodes();
    DijkstraResult r;
    r.dist.assign(n, INF_DIST);
    r.parent.assign(n, -1);
    r.nodes_explored = 0;

    if (source < 0 || source >= n) return r;

    using PQNode = std::pair<long long, int>;
    std::priority_queue<PQNode, std::vector<PQNode>, std::greater<PQNode>> pq;

    r.dist[source] = 0;
    pq.push({0, source});

    std::vector<bool> settled(n, false);

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();

        if (settled[u]) continue;
        settled[u] = true;
        r.nodes_explored++;

        if (u == target) return r;

        for (const Edge& e : g.neighbors(u)) {
            long long nd = d + e.weight;
            if (nd < r.dist[e.to]) {
                r.dist[e.to] = nd;
                r.parent[e.to] = u;
                pq.push({nd, e.to});
            }
        }
    }
    return r;
}

std::vector<int> reconstruct_path(const std::vector<int>& parent, int source, int target) {
    std::vector<int> path;
    if (target < 0 || target >= (int)parent.size()) return path;
    if (parent[target] == -1 && target != source) return path;

    int cur = target;
    while (cur != -1) {
        path.push_back(cur);
        if (cur == source) break;
        cur = parent[cur];
    }
    if (path.empty() || path.back() != source) return {};
    std::reverse(path.begin(), path.end());
    return path;
}
