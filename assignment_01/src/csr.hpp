#ifndef CSR_HPP
#define CSR_HPP

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

// One vertex's adjacency entry: neighbor id and (optional) edge weight.
// For unweighted graphs (BFS/DFS), weight is unused/ignored.
struct Edge
{
  int to;
  double weight;
};

// Adjacency-list representation, as read directly from the input file.
struct AdjacencyList
{
  int V = 0;
  int E = 0;
  bool weighted = false;
  int source = 0;
  std::vector<std::vector<Edge>> adj; // adj[u] = list of (to, weight)
};

// Compressed Sparse Row representation of the same graph.
//   row_ptr has size V+1. Neighbors of vertex u are:
//     col_idx[row_ptr[u] .. row_ptr[u+1]-1]
//   values[i] is the weight for the edge at col_idx[i] (if weighted).
struct CSRGraph
{
  int V = 0;
  int E = 0; // number of directed entries stored (== col_idx.size())
  std::vector<int> row_ptr;
  std::vector<int> col_idx;
  std::vector<double> values; // empty if unweighted
};

// ---- File I/O (NOT timed) ----

// Reads an adjacency-list graph file in the BFS/DFS format:
//   V E
//   u degree n1 n2 ...
//   ...
//   SOURCE s
// or the SSSP (weighted) format:
//   V E
//   u degree n1 w1 n2 w2 ...
//   ...
//   SOURCE s
AdjacencyList read_adjacency_list(const std::string &path, bool weighted);

// ---- CSR conversion (preprocessing — must be called BEFORE the timer
//      starts for any algorithm that consumes CSR input) ----
CSRGraph convert_to_csr(const AdjacencyList &list);

#endif // CSR_HPP