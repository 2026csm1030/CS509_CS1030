# CS509 Laboratory Repository — Individual Tasks (Assignment 2)

This repository contains the C++ implementations and driver programs. The focus of this assignment is on Single-Source Shortest Path (SSSP) using the **Bellman-Ford Algorithm** on sparse CSR graphs, and All-Pairs Shortest Path (APSP) using the **Floyd-Warshall Algorithm** on dense adjacency matrices. Both algorithms accommodate negative edge weights and handle negative-weight cycle detection.

## Student Details

* **Student Name:** Prashik
* **Entry Number:** 2026CSM1030
* **Work Mode:** Individual

## Language and Environment

* **Programming Language:** C++ (C++17 standard)
* **Compiler:** `g++` (GCC) 9.0+ or `clang++`
* **Compilation Flags:** `-std=c++17 -O2 -Wall`
* **Timing Mechanism:** `std::chrono::high_resolution_clock` (reported in milliseconds `ms`)
* **Operating System:** Linux / POSIX-compliant system

## Directory Structure

```
CS509_[EntryNumber]/
│
├── README.md                          # Comprehensive documentation and results
├── common_wrapper/
│   └── wrapper.cpp                    # Interactive menu system for compilation and execution
│
├── assignment_01/
│   ├── src/
│   │   ├── csr.hpp                    # Adjacency list & CSR graph structures
│   │   └── csr.cpp                    # Input parsing & CSR conversion routines
│   └── ...
│
└── assignment_02/
├── src/
│   ├── bellman_ford.hpp           # Bellman-Ford SSSP header
│   ├── bellman_ford.cpp           # Bellman-Ford CSR implementation
│   ├── floyd_warshall.hpp         # Floyd-Warshall APSP header
│   └── floyd_warshall.cpp         # Floyd-Warshall Matrix implementation
│
├── driver/
│   ├── bellman_ford_driver.cpp    # Standalone driver for Bellman-Ford
│   └── floyd_warshall_driver.cpp  # Standalone driver for Floyd-Warshall
│
├── tests/                         # Input test files (.txt)
│   ├── bf_10.txt
│   ├── bf_100.txt
│   ├── bf_10000.txt
│   ├── bf_50000.txt
│   ├── bf_100000.txt
│   ├── fw_10.txt
│   ├── fw_100.txt
│   ├── fw_500.txt
│   ├── fw_1000.txt
│   └── fw_2000.txt

```

## Assignment 02 — Bellman-Ford & Floyd-Warshall

### Objective

1. **Bellman-Ford Algorithm:** Implement Single-Source Shortest Path (SSSP) on directed, weighted graphs using the Compressed Sparse Row (CSR) format. Handle negative edge weights and detect reachable negative-weight cycles.
2. **Floyd-Warshall Algorithm:** Implement All-Pairs Shortest Path (APSP) using Dynamic Programming on dense $V \times V$ adjacency matrices. Handle negative edge weights and detect negative-weight cycles by inspecting diagonal values.
3. **Cross-Algorithm Verification:** Run Bellman-Ford from all vertices $s \in \{0, \dots, V-1\}$ on $V=10$ and $V=100$ graphs and cross-verify results against the corresponding rows in the Floyd-Warshall distance matrix.

### Algorithm / Approach

#### 1. Bellman-Ford (CSR Format)

* **Data Structure:** The directed graph is represented using Compressed Sparse Row (CSR) format with three contiguous arrays: `row_ptr`, `col_idx`, and `values`.
* **Execution Steps:**
1. Initialize distance array `dist[0..V-1]` to $\infty$, setting `dist[source] = 0`.
2. Perform $V - 1$ relaxation passes. For each vertex $u$, iterate over its outgoing edges in `col_idx[row_ptr[u] ... row_ptr[u+1]-1]` with weights in `values`:

$$\text{if } dist[u] + weight < dist[v], \text{ then } dist[v] = dist[u] + weight$$


3. Include an early-exit flag if no distance value updates occur during a full pass.
4. Perform a $V$-th pass. If any edge can still be relaxed, flag `has_negative_cycle = true` and omit distance output.


* **Timing Boundary:** Measurement begins immediately before step 1 and stops immediately after the $V$-th cycle-detection pass completes. File I/O, parsing, and CSR conversion are strictly excluded.

#### 2. Floyd-Warshall (Adjacency Matrix)

* **Data Structure:** Direct dense $V \times V$ double-precision matrix.
* **Execution Steps:**
1. Initialize distance matrix `dist[V][V]` directly from the input matrix file (`INF` for non-edges, `0` for diagonal).
2. Run the triple-nested dynamic programming loop for all intermediate nodes $k$, source $i$, and target $j$:

$$dist[i][j] = \min(dist[i][j], dist[i][k] + dist[k][j])$$


3. After the DP computation completes, inspect diagonal entries $dist[i][i]$. If any $dist[i][i] < 0$, flag `has_negative_cycle = true`.

* **Timing Boundary:** Measurement begins after the input matrix is fully loaded into memory and stops immediately after the diagonal check finishes.

### Input Formats

#### Bellman-Ford Adjacency-List Format (`bf_*.txt`)

Directed weighted graph with optional negative weights, followed by `SOURCE` declaration:

```
V E
u0 degree neighbor1 weight1 neighbor2 weight2 ...
u1 degree neighbor1 weight1 neighbor2 weight2 ...
...
u(V-1) degree neighbor1 weight1 neighbor2 weight2 ...
SOURCE s
```

#### Floyd-Warshall Adjacency-Matrix Format (`fw_*.txt`)

Dense $V \times V$ matrix with explicit `INF` tokens:

```
V
row 0 values (space-separated integers or INF)
row 1 values
...
row (V-1) values
```

### Helper Functions & CSR Conversion

* `read_adjacency_list(path, weighted)`: Parses weighted directed graph adjacency list from text file.
* `convert_to_csr(adj_list)`: Transforms `AdjacencyList` structure into flat `CSRGraph` arrays (`row_ptr`, `col_idx`, `values`). Executed strictly as preprocessing outside timed regions.
* `read_adjacency_matrix(path, V)`: Reads dense $V \times V$ string tokens into 2D floating-point vector.

### File Structure

```
assignment_02/
├── src/
│   ├── bellman_ford.hpp
│   ├── bellman_ford.cpp
│   ├── floyd_warshall.hpp
│   └── floyd_warshall.cpp
├── driver/
│   ├── bellman_ford_driver.cpp
│   └── floyd_warshall_driver.cpp
└── tests/
    ├── bf_10.txt ... bf_100000.txt
    └── fw_10.txt ... fw_2000.txt
```

### Compilation & Execution Commands

#### Direct Compilation

```bash
# Compile Bellman-Ford Driver
g++ -std=c++17 -O2 -I. \
  assignment_02/driver/bellman_ford_driver.cpp \
  assignment_02/src/bellman_ford.cpp \
  assignment_01/src/csr.cpp \
  -o assignment_02/bf_driver

# Compile Floyd-Warshall Driver
g++ -std=c++17 -O2 -I. \
  assignment_02/driver/floyd_warshall_driver.cpp \
  assignment_02/src/floyd_warshall.cpp \
  -o assignment_02/fw_driver
```

#### Direct Execution Examples

```bash
# Single test case execution
./assignment_02/bf_driver assignment_02/tests/bf_10.txt
./assignment_02/fw_driver assignment_02/tests/fw_10.txt

# Run all test cases in directory
./assignment_02/bf_driver --all assignment_02/tests
./assignment_02/fw_driver --all assignment_02/tests
```

### Test Cases and Result Tables

| Algorithm | Test File | Vertices ($V$) | Edges ($E$) | Source ($s$) | Input Type | Negative Cycle | Expected Output | Actual Output | Algorithm Time | Status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Bellman-Ford | `bf_10.txt` | 10 | 30 | 0 | CSR Adjacency List | No | Shortest Distances | Shortest Distances | 0.001 ms | Pass |
| Bellman-Ford | `bf_100.txt` | 100 | 400 | 0 | CSR Adjacency List | No | Shortest Distances | Shortest Distances | 0.009 ms | Pass |
| Bellman-Ford | `bf_10000.txt` | 10,000 | 30,000 | 0 | CSR Adjacency List | No | Shortest Distances | Shortest Distances | 1.709 ms | Pass |
| Bellman-Ford | `bf_50000.txt` | 50,000 | 150,000 | 0 | CSR Adjacency List | No | Shortest Distances | Shortest Distances | 14.113 ms | Pass |
| Bellman-Ford | `bf_100000.txt` | 100,000 | 300,000 | 0 | CSR Adjacency List | No | Shortest Distances | Shortest Distances | 24.798 ms | Pass |
| Floyd-Warshall | `fw_10.txt` | 10 | 100 | N/A | Dense Matrix | No | $10 \times 10$ Matrix | $10 \times 10$ Matrix | 0.004 ms | Pass |
| Floyd-Warshall | `fw_100.txt` | 100 | 10,000 | N/A | Dense Matrix | No | $100 \times 100$ Matrix | $100 \times 100$ Matrix | 0.962 ms | Pass |
| Floyd-Warshall | `fw_500.txt` | 500 | 250,000 | N/A | Dense Matrix | No | $500 \times 500$ Matrix | $500 \times 500$ Matrix | 103.028 ms | Pass |
| Floyd-Warshall | `fw_1000.txt` | 1,000 | 1,000,000 | N/A | Dense Matrix | No | $1000 \times 1000$ Matrix | $1000 \times 1000$ Matrix | 904.866 ms | Pass |
| Floyd-Warshall | `fw_2000.txt` | 2,000 | 4,000,000 | N/A | Dense Matrix | No | $2000 \times 2000$ Matrix | $2000 \times 2000$ Matrix | 7639.550 ms | Pass |

#### Cross-Verification Log ($V=10$ and $V=100$)

Running Bellman-Ford from each vertex $s \in [0, V-1]$ matches row $s$ of the Floyd-Warshall distance matrix identically for both `10` and `100` vertex graph configurations.

### Complexity Analysis

| Algorithm | Time Complexity (Best) | Time Complexity (Worst) | Space Complexity |
| --- | --- | --- | --- |
| **Bellman-Ford (CSR)** | $O(E)$ (early termination) | $O(V \cdot E)$ | $O(V + E)$ |
| **Floyd-Warshall** | $O(V^3)$ | $O(V^3)$ | $O(V^2)$ |

