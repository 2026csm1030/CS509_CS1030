# Vertex Coloring & PageRank

This directory contains the implementations and execution benchmarks for the individual tasks of Assignment 4 in CS509 (PG Software Lab). The algorithms implemented are **Greedy Vertex Coloring (Welsh-Powell Ordering)** and **PageRank**, both operating on Compressed Sparse Row (CSR) graph data structures pre-converted from adjacency lists.

## Directory Structure

```text
.
├── common_wrapper/
│   └── wrapper.cpp             # Central repository menu interface
├── assignment_01/
│   └── src/
│       ├── csr.hpp             # CSR structure and headers from Assignment 1
│       └── csr.cpp             # Adjacency list parser and CSR conversion logic
└── assignment_04/
    ├── README.md               # Assignment documentation and result tables
    ├── src/
    │   ├── vertex_coloring.hpp # Vertex Coloring headers and result structs
    │   ├── vertex_coloring.cpp # Welsh-Powell Greedy Graph Coloring logic
    │   ├── pagerank.hpp        # PageRank headers and parameters parser
    │   └── pagerank.cpp        # Power iteration PageRank solver
    ├── driver/
    │   ├── vertex_coloring_driver.cpp  # Dedicated driver for Vertex Coloring
    │   └── pagerank_driver.cpp         # Dedicated driver for PageRank
    └── tests/
        ├── color_10.txt        # Unweighted, undirected adjacency list files
        ├── color_100.txt
        ├── color_10000.txt
        ├── color_50000.txt
        ├── color_100000.txt
        ├── pagerank_10.txt     # Unweighted, directed adjacency list files
        ├── pagerank_100.txt
        ├── pagerank_1000.txt
        ├── pagerank_10000.txt
        └── pagerank_50000.txt

```

## Section 1: Vertex Coloring (Graph Coloring)

### Objective

Assign a color index to every vertex of an unweighted, undirected CSR graph such that no two adjacent vertices share the same color, using a heuristic to minimize the total number of colors used.

### Algorithm / Approach (Welsh-Powell Ordering)

1. **Degree Calculation:** Compute the degree of every vertex directly from the CSR representation: $\text{degree}(u) = \text{row\_ptr}[u+1] - \text{row\_ptr}[u]$.
2. **Ordering:** Sort vertices in non-increasing order of their degrees (Welsh-Powell heuristic).
3. **Greedy Coloring:** Iteratively process vertices in sorted order. For a vertex $u$, check the colors assigned to all its neighbors present in `col_idx`. Assign $u$ the smallest non-negative integer color index not currently used by any of its neighbors.


### Input Format

An unweighted, undirected adjacency list text file:

```text
V E
u0 degree neighbor1 neighbor2 ...
u1 degree neighbor1 neighbor2 ...
...
```

### Constraints & Validation Rules

* Edge count $E$ counts each undirected edge once.
* Self-loops are rejected.
* Isolated vertices (`degree = 0`) are assigned color `0`.

### Time & Space Complexity

* **Time Complexity:** $O(V \log V + V + E)$ where $O(V \log V)$ is required for sorting degrees and $O(V + E)$ for adjacency checks.
* **Space Complexity:** $O(V)$ auxiliary space for color tracking and ordering structures.

## Section 2: PageRank

### Objective

Estimate the relative structural importance (rank) of each vertex in an unweighted, directed CSR graph based on incoming link distributions and a damping factor.

### Algorithm / Approach (Power Iteration)

1. **Initialization:** Compute out-degrees from `row_ptr`. Initialize rank $PR(u) = \frac{1}{N}$ for all $u \in V$.
2. **Dangling Vertex Handling:** In each iteration, sum ranks of all dangling vertices ($\text{outdegree} = 0$) and distribute their mass evenly: $\text{dangling\_contrib} = d \times \frac{\sum PR_{\text{dangling}}}{N}$.
3. **Rank Propagation:** Update ranks using:
$$PR(v) = \frac{1-d}{N} + \text{dangling\_contrib} + d \sum_{u \to v} \frac{PR(u)}{\text{outdegree}(u)}$$
4. **Convergence Check:** Compute total absolute rank change $\sum_{i=0}^{N-1} \vert{}PR_{\text{new}}(i) - PR_{\text{old}}(i)\vert{}$. Terminate when change $\le \text{TOLERANCE}$ or $\text{iterations} == \text{MAX\_ITERATIONS}$.

### Input Format

An unweighted, directed adjacency list text file containing algorithm parameters:

```text
V E
u0 outdegree neighbor1 neighbor2 ...
u1 outdegree neighbor1 neighbor2 ...
...
DAMPING 0.85
TOLERANCE 0.0001
MAX_ITERATIONS 100
```

### Constraints & Validation Rules

* Rejects non-positive damping factors, damping factors $\ge 1$, non-positive tolerances, or non-positive maximum iterations.
* Ensures sum of all PageRank values remains $\approx 1.0$.

### Time & Space Complexity

* **Time Complexity:** $O(I \times (V + E))$ where $I$ is the number of iterations until convergence.
* **Space Complexity:** $O(V)$ auxiliary space for rank storage vectors.

## Compilation & Execution Instructions

### Option 1: Using Common Wrapper (Recommended)

Build and run through the central interactive interface:

```bash
# Build the wrapper from root
g++ -std=c++17 -O2 -o common_wrapper common_wrapper/wrapper.cpp

# Run the wrapper
./common_wrapper
```

### Option 2: Direct Terminal Execution

#### Compiling Vertex Coloring Driver

```bash
g++ -std=c++17 -O2 \
  assignment_04/driver/vertex_coloring_driver.cpp \
  assignment_04/src/vertex_coloring.cpp \
  assignment_01/src/csr.cpp \
  -o assignment_04/vertex_coloring_driver
```

#### Running Vertex Coloring

```bash
# Single test case
./assignment_04/vertex_coloring_driver assignment_04/tests/color_10.txt

# All test cases
./assignment_04/vertex_coloring_driver --all assignment_04/tests
```

#### Compiling PageRank Driver

```bash
g++ -std=c++17 -O2 \
  assignment_04/driver/pagerank_driver.cpp \
  assignment_04/src/pagerank.cpp \
  assignment_01/src/csr.cpp \
  -o assignment_04/pagerank_driver
```

#### Running PageRank

```bash
# Single test case
./assignment_04/pagerank_driver assignment_04/tests/pagerank_10.txt

# All test cases
./assignment_04/pagerank_driver --all assignment_04/tests
```

## Test Cases and Result Tables

> **Note on Timing Rules:** All measured times include **only algorithm execution duration**. File reading, string parsing, and Adjacency List to CSR conversions are strictly excluded from the recorded timings.

### 10.1 Vertex Coloring Results Table

| File | V | E | Colors Used | Valid? | Time | Status |
| --- | --- | --- | --- | --- | --- | --- |
| `color_10.txt` | 10 | 20 | 4 | Yes | 0.0035 ms | Pass |
| `color_100.txt` | 100 | 300 | 5 | Yes | 0.0113 ms | Pass |
| `color_10000.txt` | 10,000 | 30,000 | 6 | Yes | 0.8262 ms | Pass |
| `color_50000.txt` | 50,000 | 150,000 | 6 | Yes | 7.3618 ms | Pass |
| `color_100000.txt` | 100,000 | 300,000 | 6 | Yes | 14.9698 ms | Pass |

### 10.2 PageRank Results Table

| File | V | E | Damping | Top Vertex | Sum of Ranks | Iter. / Time | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `pagerank_10.txt` | 10 | 20 | 0.85 | 0 | ~1.000 | 19 / 0.0025 ms | Pass |
| `pagerank_100.txt` | 100 | 300 | 0.85 | 11 | ~1.000 | 14 / 0.0153 ms | Pass |
| `pagerank_1000.txt` | 1,000 | 3,000 | 0.85 | 318 | ~1.000 | 15 / 1450 ms | Pass |
| `pagerank_10000.txt` | 10,000 | 30,000 | 0.85 | 4348 | ~1.000 | 16 / 1.500 ms | Pass |
| `pagerank_50000.txt` | 50,000 | 150,000 | 0.85 | 35180 | ~1.000 | 16 / 17.1816 ms | Pass |
