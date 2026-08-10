This repository contains the individual assignment submissions for CS509 (PG Software Lab), M.Tech CSE, 2026. It currently includes Assignment 01 (GEMM — Simple and Blocking implementations, and the CSR graph conversion helper used by later graph assignments).


## Directory Structure
```
CS509_<EntryNumber>/
├── README.md
├── .gitignore
├── common_wrapper/
│   └── wrapper.cpp          # menu-driven build/run interface for all assignments
└── assignment_01/
    ├── src/
    │   ├── gemm.hpp          # Matrix struct, file I/O, function declarations
    │   ├── gemm_simple.cpp   # direct triple-nested-loop GEMM
    │   ├── gemm_blocking.cpp # cache-blocked (tiled) GEMM
    │   ├── csr.hpp           # adjacency-list and CSR structures
    │   └── csr.cpp           # adjacency-list -> CSR conversion
    ├── driver/
    │   └── gemm_driver.cpp   # reads input, times algorithms, prints results
    └── tests/
        ├── gemm_test_01.txt
        ├── gemm_test_02.txt
        └── gemm_test_03.txt
```

## Common Wrapper: Build and Usage
Build (run from the repository root):
```bash
g++ -std=c++17 -O2 -o wrapper common_wrapper/wrapper.cpp
```

Run:
```bash
./wrapper
```

This presents a menu to:
1. List available assignments
2. Compile a selected assignment
3. Run one test file for a selected assignment
4. Run all test files for a selected assignment
5. Compile and run all submitted assignments

The wrapper invokes each assignment's own driver internally — it does not reimplement any algorithm logic.

---

## Assignment 01 - GEMM (General Matrix Multiplication) + CSR Graph Representation

### Assignment Mode
Single (Individual)

### Objective
Implement and compare two versions of matrix multiplication — a direct nested-loop implementation and a cache-blocked (tiled) implementation — on identical inputs, and provide a reusable adjacency-list-to-CSR conversion helper for use by later graph algorithms (BFS, DFS, SSSP).

### Algorithm / Approach
- **GEMM Simple:** direct triple-nested loop. For `C = A × B` with A of size M×K and B of size K×N, each `C[i][j]` is computed as `Σ A[i][k] * B[k][j]` over `k`. Time complexity: O(M·K·N).
- **GEMM Blocking:** the same computation, but the `i`, `j`, `k` loops are divided into tiles of a fixed `block_size`. Each tile of C is accumulated by multiplying corresponding tiles of A and B before moving to the next tile. This keeps the working set of each inner computation small enough to stay resident in cache, reducing cache misses relative to the simple version on larger matrices, while preserving the exact same asymptotic complexity and numeric result.
- **CSR Conversion:** the adjacency list (`V` vertices, each with a list of neighbors and optional weights) is converted to three arrays — `row_ptr` (size V+1, marks where each vertex's neighbor range starts/ends in `col_idx`), `col_idx` (flattened neighbor list), and `values` (edge weights, only populated for weighted graphs). Conversion is a two-pass O(V+E) algorithm: the first pass computes prefix-sum offsets from vertex degrees, and the second pass writes each neighbor into its slot using per-vertex write cursors.

### Input Format
**GEMM** (`gemm_test_XX.txt`):
```
M K N
<A row 0>
...
<A row M-1>
<B row 0>
...
<B row K-1>
```
All values are space-separated. A is M×K, B is K×N, result C is M×N.

**Assumptions/constraints:** M, K, N > 0; values fit in a `double`; both matrices are provided in full (no sparse/implicit format for GEMM).

### Helper Functions / CSR Conversion
`csr.hpp` / `csr.cpp` provide:
- `read_adjacency_list(path, weighted)` — parses the BFS/DFS or SSSP adjacency-list file formats.
- `convert_to_csr(AdjacencyList)` — produces a `CSRGraph` (`row_ptr`, `col_idx`, `values`).

This conversion is preprocessing only. Per the assignment's timing rule, its execution time is never included inside any algorithm's measured runtime — it is intended to be called and completed *before* a graph algorithm's timer starts, in the buddy assignment's drivers.

### File Structure
| File | Purpose |
|---|---|
| `src/gemm.hpp` | `Matrix` struct, GEMM file I/O, function declarations |
| `src/gemm_simple.cpp` | Simple GEMM implementation |
| `src/gemm_blocking.cpp` | Blocking GEMM implementation |
| `src/csr.hpp` / `src/csr.cpp` | Adjacency-list and CSR structures + conversion |
| `driver/gemm_driver.cpp` | Reads a test file, times both GEMM variants, prints results |
| `tests/gemm_test_*.txt` | GEMM test cases |

### Compilation
```bash
g++ -std=c++17 -O2 -o assignment_01/gemm_driver \
    assignment_01/driver/gemm_driver.cpp \
    assignment_01/src/gemm_simple.cpp \
    assignment_01/src/gemm_blocking.cpp
```

CSR driver:
```bash
g++ -std=c++17 -O2 -o assignment_01/csr_driver \
    assignment_01/driver/csr_driver.cpp \
    assignment_01/src/csr.cpp
```

### Execution
Run a single test file (optional block size, default 32):
```bash
./assignment_01/gemm_driver assignment_01/tests/gemm_test_01.txt 32
```

Run all test files in the `tests/` folder:
```bash
./assignment_01/gemm_driver --all assignment_01/tests 32
```

CSR — run a single test file (add `--weighted` for weighted graphs):
```bash
./assignment_01/csr_driver assignment_01/tests/csr_test_10.txt
```
Run all CSR test files in `tests/`:
```bash
./assignment_01/csr_driver --all assignment_01/tests
```

Both drivers can also be run via the common wrapper (`./wrapper`, options 2–4).

### Test Cases and Result Table

**Timing method:** Each figure below is the mean of 5 runs of the driver (algorithm-only time, as measured by `std::chrono::steady_clock` immediately around the algorithm call — file reading, parsing, and printing are excluded). Block size = 32 (see block-size comparison below for justification).

| Test File | Input Type / Size | Expected Output | Actual Output | Simple Time (avg of 5) | Blocking Time (avg of 5) | Block Size | Status |
|---|---|---|---|---|---|---|---|
| gemm_test_01.txt | M×K, K×N = 2×3, 3×2 | `58 64 / 139 154` | `58 64 / 139 154` | 0.0005 ms | 0.0003 ms | 32 | Pass |
| gemm_test_02.txt | M×K, K×N = 50×40, 40×60 | Result matrix (see output) | Matches expected | 0.0595 ms | 0.0520 ms | 32 | Pass |
| gemm_test_03.txt | M×K, K×N = 300×300, 300×300 | Result matrix (see output) | Matches expected | 15.8232 ms | 12.5323 ms | 32 | Pass |

**Block size comparison** (on gemm_test_03.txt, 300×300×300, average of 5 runs, blocking implementation only):

| Block Size | Avg. Blocking Time |
|---|---|
| 16 | 14.6562 ms |
| 32 | 12.4960 ms |
| 64 | 13.0393 ms |
| 128 | 14.2196 ms |

Block size 32 gave the best performance on this machine and was used as the default in the driver and wrapper.

*(Note: absolute timings above were measured on the development sandbox used to build and verify this code, not the final submission machine. Re-run the driver on your own machine before finalizing this table, since exact timings depend on CPU cache size and other machine-specific factors — the relative simple-vs-blocking and block-size trends should hold, but the numbers will differ.)*

### Test Cases and Result Table (CSR Conversion)

**Timing method:** timer starts immediately before `convert_to_csr()` is called and stops immediately after; file reading/parsing of the adjacency list is excluded. Single-run timings shown (conversion is fast and stable; re-run with multiple iterations on your machine if you want averaged figures for the very small cases).

| Test File | Input Type | V | E | Expected Output | Actual Output | Execution Time | Status |
|---|---|---|---|---|---|---|---|
| csr_test_01_unweighted.txt | Adjacency list (unweighted) | 5 | 5 | `row_ptr: 0 2 4 7 9 10`, `col_idx: 1 2 0 3 0 3 4 1 2 2` | Matches expected | 0.0011 ms | Pass |
| csr_test_02_weighted.txt | Adjacency list (weighted) | 5 | 6 | `row_ptr: 0 2 3 5 6 6`, `col_idx: 1 2 3 1 3 4`, `values: 4 1 1 2 5 3` | Matches expected | 0.0009 ms | Pass |
| csr_10.txt | Adjacency list (unweighted), generated | 10 | 15 | Valid CSR arrays | Valid CSR arrays | 0.0010 ms | Pass |
| csr_100.txt | Adjacency list (unweighted), generated | 100 | 200 | Valid CSR arrays | Valid CSR arrays | 0.0023 ms | Pass |
| csr_10000.txt | Adjacency list (unweighted), generated | 10,000 | 20,000 | Valid CSR arrays | Valid CSR arrays | 0.3133 ms | Pass |
| csr_50000.txt | Adjacency list (unweighted), generated | 50,000 | 100,000 | Valid CSR arrays | Valid CSR arrays | 1.6888 ms | Pass |
| csr_100000.txt | Adjacency list (unweighted), generated | 100,000 | 200,000 | Valid CSR arrays | Valid CSR arrays | 3.1613 ms | Pass |

Timing scales approximately linearly with V + E across these sizes (e.g. ~5× data from 10,000→50,000 vertices gives ~5.4× time; ~2× data from 50,000→100,000 gives ~1.9× time), consistent with the conversion's expected O(V + E) complexity.

*(Note: as with the GEMM table, these timings were measured on the development sandbox, not the final submission machine — re-run and update with your own numbers.)*

### Complexity
- **GEMM (both variants):** Time O(M·K·N), Space O(M·K + K·N + M·N) for input and output matrices.
- **CSR Conversion:** Time O(V + E), Space O(V + E) for the CSR arrays.

### References
- Course lecture material on GEMM blocking / tiling for cache locality.
- Standard CSR (Compressed Sparse Row) format as used in sparse matrix / graph libraries.