
# EDA — Práctica Final Integradora

Análisis de la red vial de Pennsylvania (roadNet-PA, dataset SNAP) mediante algoritmos sobre grafos: estadísticas estructurales, camino mínimo punto a punto con Dijkstra y BFS, y construcción de subgrafo inducido con MST y verificación DAG.

## Equipo

- Nash Díaz
- Daniel Osorio
- Alejandro Restrepo

Universidad EAFIT — Estructuras de Datos y Algoritmos 

## Dataset

- **Nombre:** roadNet-PA (Pennsylvania Road Network)
- **Fuente:** [SNAP — Stanford Network Analysis Project](https://snap.stanford.edu/data/roadNet-PA.html)
- **URL de descarga:** https://snap.stanford.edu/data/roadNet-PA.txt.gz
- **Estadísticas oficiales:** 1.088.092 nodos, 1.541.898 aristas no dirigidas
- **Citación:** Leskovec, J. & Krevl, A. (2014). *SNAP Datasets: Stanford Large Network Dataset Collection*. http://snap.stanford.edu/data

El archivo `roadNet-PA.txt` debe estar ubicado en `data/` antes de ejecutar el programa.

## Estructura del repositorio
EDA_PF_NashDiaz_DanielOsorio_AlejandroRestrepo/
├── README.md
├── data/
│   └── roadNet-PA.txt
├── results/
│   ├── analisis_estructural.txt
│   ├── consultas_p2p.csv
│   ├── camino_Q01.txt
│   ├── camino_Q06.txt
│   ├── subgrafo_caminos.txt
│   ├── analisis_subgrafo.txt
│   └── tiempos.csv
├── src/
│   ├── graph.hpp / graph.cpp
│   ├── loader.hpp / loader.cpp
│   ├── bfs.hpp / bfs.cpp
│   ├── dijkstra.hpp / dijkstra.cpp
│   ├── structural.hpp / structural.cpp
│   ├── subgraph.hpp / subgraph.cpp
│   └── main.cpp
└── report/
└── Informe.pdf

## Dataset

Bajar el archivo desde https://snap.stanford.edu/data/roadNet-PA.txt.gz, descomprimirlo y ubicarlo en data/roadNet-PA.txt antes de compilar. La fuente oficial es Leskovec, J. y Krevl, A. (2014), SNAP Datasets: Stanford Large Network Dataset Collection, http://snap.stanford.edu/data.

## Compilación

Desde la raíz del proyecto, con g++ y soporte para C++17:

g++ -std=c++17 -O2 -o eda_pf.exe src/main.cpp src/graph.cpp src/loader.cpp src/bfs.cpp src/dijkstra.cpp src/structural.cpp src/subgraph.cpp

El flag -O2 es necesario para obtener tiempos representativos sobre el dataset completo. En Linux o macOS basta con cambiar el nombre del ejecutable a eda_pf.

## Ejecución

El programa recibe dos argumentos: la ruta al dataset y el módulo a correr (A, B, C o ALL).

.\eda_pf.exe data\roadNet-PA.txt A
.\eda_pf.exe data\roadNet-PA.txt B
.\eda_pf.exe data\roadNet-PA.txt C

Cada módulo escribe sus resultados dentro de la carpeta results, que se crea automáticamente. El módulo A demora alrededor de 5 segundos, el B alrededor de un minuto y medio (corre Dijkstra y BFS sobre las 10 consultas), y el C unos pocos segundos.

## Resultados generados

Después de correr los tres módulos, en results quedan los archivos analisis_estructural.txt con las estadísticas del grafo y la comparación contra los valores publicados por SNAP, consultas_p2p.csv con la tabla de las 10 consultas y los tiempos, camino_Q01.txt y camino_Q06.txt con la secuencia completa de nodos de cada camino mínimo, subgrafo_caminos.txt con la lista de adyacencia del subgrafo inducido, analisis_subgrafo.txt con el peso del MST y el resultado del análisis DAG, y tiempos.csv con el tiempo total de cada módulo.

## Resultados destacados

| Propiedad | Valor SNAP | Valor obtenido |
|---|---|---|
| Nodos | 1.088.092 | 1.088.092 |
| Aristas no dirigidas | 1.541.898 | 1.541.898 |
| Componente conexa principal | 1.087.562 | 1.087.562 |
| Componentes conexas totales | — | 206 |
| Grado promedio | 2,83 | 2,8341 |
| Nodo de mayor grado | — | 674.502 (grado 9) |
| Diámetro | 782 | 772 (estimado, doble BFS) |

El diámetro se estima con la heurística de doble BFS, que entrega una cota inferior del valor real. La diferencia respecto al valor publicado es del 1,3%.

Para el subgrafo inducido por la unión de los caminos de Q01 y Q06 (225 nodos, 228 aristas), el MST tiene peso total 927 y el resultado del análisis DAG es positivo (0 back edges, 189 tree edges).

## Reproducibilidad

Los pesos se inicializan con srand(42) en el momento de la carga, así que cualquiera que corra el programa con el mismo dataset obtiene exactamente las mismas distancias en Dijkstra y los mismos pesos en el MST. Los IDs de nodo del archivo no son consecutivos, así que internamente el grafo los reindexa a 0..N-1 manteniendo un mapeo bidireccional para poder reportar resultados con los IDs originales.

## Notas

La lista de adyacencia ponderada ocupa entre 50 y 80 MB de RAM. Una matriz de adyacencia para este grafo requeriría aproximadamente 4,7 TB, lo que la vuelve inviable y justifica el uso de la lista. Tanto Dijkstra como BFS cortan al alcanzar el nodo destino en las consultas P2P para no recorrer toda la componente. El archivo SNAP lista cada arista no dirigida dos veces (una por dirección), y el cargador descarta los duplicados.
