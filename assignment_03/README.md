# Minimum Spanning Tree

This repository contains the C++ implementation for the individual task of **Assignment 03** in the CS509 Software Lab course. The objective of this task is to implement, compare, and analyze two fundamental greedy algorithms for computing the Minimum Spanning Tree (MST) of weighted undirected graphs: **Kruskal's Algorithm** and **Prim's Algorithm**.

Both algorithms consume graph inputs represented in **Compressed Sparse Row (CSR)** format, converted from adjacency-list text files. Algorithm execution times are precisely benchmarked while excluding input parsing, memory setup, and CSR graph construction.


## Directory Structure

```text
├── assignment_03/
│   ├── src/
│   │   ├── kruskal.hpp
│   │   ├── kruskal.cpp
│   │   ├── prim.hpp
│   │   └── prim.cpp
│   ├── driver/
│   │   ├── kruskal_driver.cpp
│   │   ├── prim_driver.cpp
│   │   └── mst_driver.cpp
│   └── tests/
│       ├── mst_example.txt
│       ├── mst_10.txt
│       ├── mst_100.txt
│       ├── mst_10000.txt
│       ├── mst_50000.txt
│       └── mst_100000.txt
```


## Algorithm / Approach

### 1. Kruskal's Algorithm

* **Strategy:** Edge-centric greedy algorithm.
* **Implementation:**
* Extracts unique undirected edges $(u, v, w)$ with $u < v$ directly from the input CSR arrays (`row_ptr`, `col_idx`, `values`).
* Sorts extracted edges in non-decreasing order of weight.
* Uses a Disjoint Set Union (DSU) data structure with path compression and union-by-rank to prevent cycle formation.
* Processes edges sequentially and terminates as soon as $V - 1$ edges are added to the MST.
* **Timed Region:** Per Section 8 requirements, edge extraction from CSR, edge sorting, and DSU union operations are included inside the measured time region.

### 2. Prim's Algorithm

* **Strategy:** Vertex-centric greedy algorithm.
* **Implementation:**
* Starts from vertex 0 to ensure reproducibility.
* Maintains a boolean vector `in_mst` to track visited vertices and a min-priority queue (`std::priority_queue`) storing reachable boundary edges.
* Interrogates `row_ptr` and `col_idx` to push adjacent unvisited edges into the min-heap.
* Repeatedly extracts the minimum-weight edge connecting an MST vertex to a non-MST vertex until all $V$ vertices are connected ($V - 1$ edges selected).
* **Timed Region:** Priority queue initialization, vertex visitation tracking, and edge pushes/pops take place strictly inside the measured region.

### Input Format and Assumptions

* **Format:** Weighted undirected adjacency-list text format (`.txt`).


* **Structure:**
```text
V E
u0 degree neighbor1 weight1 neighbor2 weight2 ...
u1 degree neighbor1 weight1 neighbor2 weight2 ...
...
```


* **Assumptions:**

* Graph vertices are indexed $0$ to $V-1$.
* Each undirected edge appears in the adjacency list of both endpoints with matching weight.
* Graph is connected ($V > 1$, no isolated vertices).
* Total edge count $E$ counts each undirected edge once.


### Source and Driver Files

* `assignment_01/src/csr.hpp` & `csr.cpp`: Data structures and conversion routines for CSR format.
* `assignment_03/src/kruskal.hpp` & `kruskal.cpp`: DSU implementation and Kruskal's MST algorithm.
* `assignment_03/src/prim.hpp` & `prim.cpp`: Min-heap Prim's MST algorithm.
* `assignment_03/driver/kruskal_driver.cpp`: Dedicated driver for Kruskal's algorithm.
* `assignment_03/driver/prim_driver.cpp`: Dedicated driver for Prim's algorithm.
* `assignment_03/driver/mst_driver.cpp`: Combined comparison driver running both algorithms on the same graph input.

### Compilation Instructions

From the repository root directory, run:

#### Compile Dedicated Kruskal Driver

```bash
g++ -std=c++17 -O2 -o assignment_03/kruskal_driver \
    assignment_03/driver/kruskal_driver.cpp \
    assignment_03/src/kruskal.cpp \
    assignment_01/src/csr.cpp
```

#### Compile Dedicated Prim Driver

```bash
g++ -std=c++17 -O2 -o assignment_03/prim_driver \
    assignment_03/driver/prim_driver.cpp \
    assignment_03/src/prim.cpp \
    assignment_01/src/csr.cpp
```

#### Compile Unified MST Comparison Driver

```bash
g++ -std=c++17 -O2 -o assignment_03/mst_driver \
    assignment_03/driver/mst_driver.cpp \
    assignment_03/src/kruskal.cpp \
    assignment_03/src/prim.cpp \
    assignment_01/src/csr.cpp
```

### Execution Instructions

#### Run Single Test File

```bash
./assignment_03/mst_driver assignment_03/tests/mst_10.txt
```

#### Run All Test Files in Directory

```bash
./assignment_03/mst_driver --all assignment_03/tests
```

### Test Cases and Result Table

Input graph test files are generated using `generate_mst_tests.py` with sparse densities ($E \approx 2.5V$).

| File | $V$ | $E$ | Exp. Wt. | Kruskal Wt. | Prim Wt. | Kruskal Time (ms) | Prim Time (ms) | Equal? | Status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `mst_10.txt` | 10 | 20 |  | 43 | 43 | 0.0020 ms | 0.0024 ms |  | Pass |
| `mst_100.txt` | 100 | 250 |  | 495 | 495 | 0.0276 ms | 0.0326 ms |  | Pass |
| `mst_10000.txt` | 10,000 | 25,000 |  | 51836 | 51836 | 2.7850 ms | 5.2485 ms |  | Pass |
| `mst_50000.txt` | 50,000 | 125,000 |  | 259459 | 259459 | 15.2247 ms | 36.8231 ms |  | Pass |
| `mst_100000.txt` | 100,000 | 250,000 |  | 519593 | 519593 | 30.9400 ms | 85.4377 ms |  | Pass |


### Time and Space Complexity

| Algorithm | Data Structure | Time Complexity | Auxiliary Space Complexity |
| --- | --- | --- | --- |
| **Kruskal** | Disjoint Set Union (Path Compression + Rank) | $\mathcal{O}(E \log E)$ or $\mathcal{O}(E \log V)$<br> | $\mathcal{O}(V + E)$<br> |
| **Prim** | Min-Binary Heap (`std::priority_queue`) | $\mathcal{O}(E \log V)$<br> | $\mathcal{O}(V + E)$<br> |