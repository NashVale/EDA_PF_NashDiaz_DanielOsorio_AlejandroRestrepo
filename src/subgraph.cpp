
#include "subgraph.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <functional>

class UnionFind {
    std::vector<int> parent, rank_;
public:
    UnionFind(int n) : parent(n), rank_(n, 0) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return false;
        if (rank_[ra] < rank_[rb]) std::swap(ra, rb);
        parent[rb] = ra;
        if (rank_[ra] == rank_[rb]) rank_[ra]++;
        return true;
    }
};

Subgraph build_induced_subgraph(const Graph& g, const std::vector<int>& path_nodes) {
    Subgraph sg;
    std::unordered_set<int> node_set(path_nodes.begin(), path_nodes.end());
    sg.nodes_internal.assign(node_set.begin(), node_set.end());
    std::sort(sg.nodes_internal.begin(), sg.nodes_internal.end());

    std::unordered_set<long long> seen;
    auto key = [](int a, int b) -> long long {
        if (a > b) std::swap(a, b);
        return ((long long)a << 32) | (unsigned int)b;
    };

    for (int u : sg.nodes_internal) {
        for (const Edge& e : g.neighbors(u)) {
            if (node_set.count(e.to)) {
                long long k = key(u, e.to);
                if (!seen.count(k)) {
                    seen.insert(k);
                    sg.edges.push_back({u, e.to, e.weight});
                }
            }
        }
    }
    return sg;
}

MSTResult kruskal_mst(const Subgraph& sg) {
    MSTResult r{0, 0};

    std::unordered_map<int, int> remap;
    for (int i = 0; i < (int)sg.nodes_internal.size(); i++) {
        remap[sg.nodes_internal[i]] = i;
    }

    std::vector<SubgraphEdge> sorted_edges = sg.edges;
    std::sort(sorted_edges.begin(), sorted_edges.end(),
              [](const SubgraphEdge& a, const SubgraphEdge& b) {
                  return a.weight < b.weight;
              });

    UnionFind uf((int)sg.nodes_internal.size());
    for (const auto& e : sorted_edges) {
        int a = remap[e.u_internal];
        int b = remap[e.v_internal];
        if (uf.unite(a, b)) {
            r.total_weight += e.weight;
            r.num_edges++;
        }
    }
    return r;
}

DAGResult check_dag_directed(const Subgraph& sg) {
    DAGResult r{true, 0, 0};

    std::unordered_map<int, int> remap;
    for (int i = 0; i < (int)sg.nodes_internal.size(); i++) {
        remap[sg.nodes_internal[i]] = i;
    }
    int n = (int)sg.nodes_internal.size();

    std::vector<std::vector<int>> dir_adj(n);
    for (const auto& e : sg.edges) {
        dir_adj[remap[e.u_internal]].push_back(remap[e.v_internal]);
    }

    std::vector<int> color(n, 0);

    std::function<void(int)> dfs = [&](int u) {
        color[u] = 1;
        for (int v : dir_adj[u]) {
            if (color[v] == 0) {
                r.tree_edges++;
                dfs(v);
            } else if (color[v] == 1) {
                r.back_edges++;
                r.is_dag = false;
            }
        }
        color[u] = 2;
    };

    for (int i = 0; i < n; i++) {
        if (color[i] == 0) dfs(i);
    }
    return r;
}

void save_subgraph(const Subgraph& sg, const Graph& g, const std::string& filename) {
    std::ofstream f(filename);
    f << "# Subgrafo inducido por nodos de los caminos Q01 y Q06\n";
    f << "# FromNodeId\tToNodeId\n";
    for (const auto& e : sg.edges) {
        f << g.get_original(e.u_internal) << "\t" << g.get_original(e.v_internal) << "\n";
    }
}

void save_subgraph_analysis(const Subgraph& sg, const MSTResult& mst,
                             const DAGResult& dag, const std::string& filename) {
    std::ofstream f(filename);
    f << "ANALISIS DEL SUBGRAFO INDUCIDO\n";
    f << "===============================\n\n";
    f << "Numero de nodos: " << sg.nodes_internal.size() << "\n";
    f << "Numero de aristas: " << sg.edges.size() << "\n\n";

    f << "ARBOL DE EXPANSION MINIMA (Kruskal)\n";
    f << "------------------------------------\n";
    f << "Aristas en el MST: " << mst.num_edges << "\n";
    f << "Peso total del MST: " << mst.total_weight << "\n\n";

    f << "VERIFICACION DAG (con clasificacion de aristas)\n";
    f << "------------------------------------------------\n";
    f << "Aristas tree: " << dag.tree_edges << "\n";
    f << "Aristas back (ciclos detectados): " << dag.back_edges << "\n";
    f << "Es DAG: " << (dag.is_dag ? "SI" : "NO") << "\n\n";

    f << "Nota: El subgrafo se trata como dirigido para la verificacion DAG,\n";
    f << "tomando la orientacion de las aristas tal como aparecen en la lista de\n";
    f << "adyacencia. Una arista no dirigida (u-v) se almacena como (u->v) y (v->u),\n";
    f << "por lo que cualquier subgrafo no trivial tendra back edges. Reportamos\n";
    f << "ambos conteos para que la clasificacion sea informativa.\n";
}
