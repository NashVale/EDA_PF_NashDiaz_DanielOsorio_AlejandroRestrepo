
#include "structural.hpp"
#include "bfs.hpp"
#include <fstream>
#include <iomanip>
#include <stack>

static int dfs_component_size(const Graph& g, int start, std::vector<bool>& visited) {
    std::stack<int> st;
    st.push(start);
    visited[start] = true;
    int size = 0;

    while (!st.empty()) {
        int u = st.top(); st.pop();
        size++;
        for (const Edge& e : g.neighbors(u)) {
            if (!visited[e.to]) {
                visited[e.to] = true;
                st.push(e.to);
            }
        }
    }
    return size;
}

static int estimate_diameter_double_bfs(const Graph& g, int start) {
    int farthest_a = start;
    bfs_eccentricity(g, start, farthest_a);
    int farthest_b = farthest_a;
    int diameter = bfs_eccentricity(g, farthest_a, farthest_b);
    return diameter;
}

GraphStats analyze_graph(const Graph& g) {
    GraphStats s{};
    s.num_nodes = g.num_nodes();
    s.num_edges = g.num_edges();

    long long total_degree = 0;
    s.max_degree = 0;
    s.max_degree_node_orig = -1;

    for (int i = 0; i < s.num_nodes; i++) {
        int deg = (int)g.neighbors(i).size();
        total_degree += deg;
        if (deg > s.max_degree) {
            s.max_degree = deg;
            s.max_degree_node_orig = g.get_original(i);
        }
    }
    s.avg_degree = (s.num_nodes > 0) ? (double)total_degree / s.num_nodes : 0.0;

    std::vector<bool> visited(s.num_nodes, false);
    s.num_components = 0;
    s.main_component_size = 0;
    int largest_root = 0;

    for (int i = 0; i < s.num_nodes; i++) {
        if (!visited[i]) {
            int sz = dfs_component_size(g, i, visited);
            s.num_components++;
            if (sz > s.main_component_size) {
                s.main_component_size = sz;
                largest_root = i;
            }
        }
    }

    s.diameter_estimate = estimate_diameter_double_bfs(g, largest_root);
    return s;
}

void save_structural_analysis(const GraphStats& s, const std::string& filename) {
    std::ofstream f(filename);
    f << std::fixed << std::setprecision(4);

    f << "ANALISIS ESTRUCTURAL DEL GRAFO roadNet-PA\n";
    f << "==========================================\n\n";

    f << "Numero de nodos: " << s.num_nodes << "\n";
    f << "Numero de aristas (no dirigidas): " << s.num_edges << "\n";
    f << "Nodos en la componente conexa principal: " << s.main_component_size << "\n";
    f << "Numero de componentes conexas: " << s.num_components << "\n";
    f << "Grado promedio: " << s.avg_degree << "\n";
    f << "Nodo de mayor grado (ID original): " << s.max_degree_node_orig
      << " (grado: " << s.max_degree << ")\n";
    f << "Diametro estimado (doble BFS): " << s.diameter_estimate << "\n\n";

    f << "COMPARACION CON VALORES SNAP\n";
    f << "----------------------------\n";
    f << "Nodos                     | SNAP: 1088092   | Obtenido: " << s.num_nodes << "\n";
    f << "Aristas (no dirigidas)    | SNAP: 1541898   | Obtenido: " << s.num_edges << "\n";
    f << "Componente conexa princ.  | SNAP: 1087562   | Obtenido: " << s.main_component_size << "\n";
    f << "Grado promedio            | SNAP: 2.83      | Obtenido: " << s.avg_degree << "\n";
    f << "Diametro                  | SNAP: 782       | Estimado: " << s.diameter_estimate << "\n";
}
