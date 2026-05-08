
#include "graph.hpp"
#include "loader.hpp"
#include "bfs.hpp"
#include "dijkstra.hpp"
#include "structural.hpp"
#include "subgraph.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

struct Query {
    std::string id;
    int src_orig;
    int dst_orig;
};

static const std::vector<Query> QUERIES = {
    {"Q01",      1,  500000},
    {"Q02",    100, 1000000},
    {"Q03",  50000,  750000},
    {"Q04", 200000,  800000},
    {"Q05", 300000,  100000},
    {"Q06",      1, 1087562},
    {"Q07", 500000,       1},
    {"Q08", 250000,  600000},
    {"Q09",  10000,  900000},
    {"Q10", 400000,  150000}
};

static double ms_since(std::chrono::high_resolution_clock::time_point t0) {
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}

static void append_timing(const std::string& filename, const std::string& mod, double ms) {
    bool exists = fs::exists(filename);
    std::ofstream f(filename, std::ios::app);
    if (!exists) f << "modulo,tiempo_ms\n";
    f << mod << "," << std::fixed << std::setprecision(2) << ms << "\n";
}

static int run_module_a(const std::string& dataset) {
    std::cout << "[Modulo A] Cargando grafo...\n";
    auto t0 = std::chrono::high_resolution_clock::now();
    Graph g = load_graph(dataset);
    std::cout << "[Modulo A] Cargado en " << ms_since(t0) << " ms. "
              << "Nodos=" << g.num_nodes() << " Aristas=" << g.num_edges() << "\n";

    std::cout << "[Modulo A] Calculando estadisticas...\n";
    auto t1 = std::chrono::high_resolution_clock::now();
    GraphStats s = analyze_graph(g);
    double t_analysis = ms_since(t1);

    fs::create_directories("results");
    save_structural_analysis(s, "results/analisis_estructural.txt");

    std::cout << "[Modulo A] Analisis: " << t_analysis << " ms\n";
    std::cout << "  Componente principal: " << s.main_component_size << "\n";
    std::cout << "  Componentes totales: " << s.num_components << "\n";
    std::cout << "  Grado promedio: " << s.avg_degree << "\n";
    std::cout << "  Nodo max grado: " << s.max_degree_node_orig << " (grado " << s.max_degree << ")\n";
    std::cout << "  Diametro estimado: " << s.diameter_estimate << "\n";

    double total = ms_since(t0);
    append_timing("results/tiempos.csv", "A", total);
    return 0;
}

static int run_module_b(const std::string& dataset) {
    std::cout << "[Modulo B] Cargando grafo...\n";
    auto t0 = std::chrono::high_resolution_clock::now();
    Graph g = load_graph(dataset);
    std::cout << "[Modulo B] Cargado en " << ms_since(t0) << " ms.\n";

    fs::create_directories("results");
    std::ofstream csv("results/consultas_p2p.csv");
    csv << "consulta,origen,destino,dist_dijkstra,saltos_bfs,"
        << "nodos_dijkstra,nodos_bfs,t_dijkstra_ms,t_bfs_ms\n";
    csv << std::fixed << std::setprecision(2);

    auto t_b_start = std::chrono::high_resolution_clock::now();

    for (const Query& q : QUERIES) {
        int src = g.find_internal(q.src_orig);
        int dst = g.find_internal(q.dst_orig);

        if (src == -1 || dst == -1) {
            csv << q.id << "," << q.src_orig << "," << q.dst_orig
                << ",INF,INF,0,0,0.00,0.00\n";
            std::cout << "  " << q.id << ": nodo no existe en el grafo\n";
            continue;
        }

        auto td0 = std::chrono::high_resolution_clock::now();
        DijkstraResult dr = dijkstra(g, src, dst);
        double t_d = ms_since(td0);

        auto tb0 = std::chrono::high_resolution_clock::now();
        BFSResult br = bfs(g, src, dst);
        double t_b = ms_since(tb0);

        std::string dist_str = (dr.dist[dst] == INF_DIST) ? "INF" : std::to_string(dr.dist[dst]);
        std::string hops_str = (br.dist[dst] == INT_MAX) ? "INF" : std::to_string(br.dist[dst]);

        csv << q.id << "," << q.src_orig << "," << q.dst_orig << ","
            << dist_str << "," << hops_str << ","
            << dr.nodes_explored << "," << br.nodes_explored << ","
            << t_d << "," << t_b << "\n";

        std::cout << "  " << q.id << ": dist=" << dist_str
                  << " saltos=" << hops_str
                  << " td=" << t_d << "ms tb=" << t_b << "ms\n";

        if (q.id == "Q01" || q.id == "Q06") {
            std::vector<int> path = reconstruct_path(dr.parent, src, dst);
            std::ofstream pf("results/camino_" + q.id + ".txt");
            pf << "# Camino reconstruido " << q.id << ": "
               << q.src_orig << " -> " << q.dst_orig << "\n";
            pf << "# Longitud (nodos): " << path.size() << "\n";
            for (int v : path) pf << g.get_original(v) << "\n";
        }
    }

    double total = ms_since(t_b_start);
    std::cout << "[Modulo B] Total queries: " << total << " ms\n";
    append_timing("results/tiempos.csv", "B", ms_since(t0));
    return 0;
}

static int run_module_c(const std::string& dataset) {
    std::cout << "[Modulo C] Cargando grafo y ejecutando Q01, Q06...\n";
    auto t0 = std::chrono::high_resolution_clock::now();

    Graph g = load_graph(dataset);

    int src1 = g.find_internal(1);
    int dst1 = g.find_internal(500000);
    int src6 = g.find_internal(1);
    int dst6 = g.find_internal(1087562);

    if (src1 == -1 || dst1 == -1 || dst6 == -1) {
        std::cerr << "Error: alguno de los nodos de Q01/Q06 no existe.\n";
        return 1;
    }

    DijkstraResult d1 = dijkstra(g, src1, dst1);
    std::vector<int> path1 = reconstruct_path(d1.parent, src1, dst1);

    DijkstraResult d6 = dijkstra(g, src6, dst6);
    std::vector<int> path6 = reconstruct_path(d6.parent, src6, dst6);

    std::vector<int> union_nodes = path1;
    union_nodes.insert(union_nodes.end(), path6.begin(), path6.end());

    Subgraph sg = build_induced_subgraph(g, union_nodes);
    MSTResult mst = kruskal_mst(sg);
    DAGResult dag = check_dag_directed(sg);

    fs::create_directories("results");
    save_subgraph(sg, g, "results/subgrafo_caminos.txt");
    save_subgraph_analysis(sg, mst, dag, "results/analisis_subgrafo.txt");

    std::cout << "[Modulo C] Subgrafo: " << sg.nodes_internal.size()
              << " nodos, " << sg.edges.size() << " aristas\n";
    std::cout << "[Modulo C] MST peso=" << mst.total_weight
              << " aristas=" << mst.num_edges << "\n";
    std::cout << "[Modulo C] DAG: " << (dag.is_dag ? "SI" : "NO")
              << " (back=" << dag.back_edges << " tree=" << dag.tree_edges << ")\n";

    append_timing("results/tiempos.csv", "C", ms_since(t0));
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <ruta_dataset> <A|B|C|ALL>\n";
        std::cerr << "Ejemplo: " << argv[0] << " data/roadNet-PA.txt A\n";
        return 1;
    }
    std::string dataset = argv[1];
    std::string mod = argv[2];

    try {
        if (mod == "A" || mod == "a") return run_module_a(dataset);
        if (mod == "B" || mod == "b") return run_module_b(dataset);
        if (mod == "C" || mod == "c") return run_module_c(dataset);
        if (mod == "ALL" || mod == "all") {
            int rc = run_module_a(dataset);
            if (rc) return rc;
            rc = run_module_b(dataset);
            if (rc) return rc;
            return run_module_c(dataset);
        }
        std::cerr << "Modulo desconocido: " << mod << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
