#ifndef CSR_HPP
#define CSR_HPP

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

struct Edge
{
  int to;
  double weight;
};

struct AdjacencyList
{
  int V = 0;
  int E = 0;
  bool weighted = false;
  int source = 0;
  vector<vector<Edge>> adj;
};

// Compressed Sparse Row representation.
struct CSRGraph
{
  int V = 0;
  int E = 0;
  vector<int> row_ptr;
  vector<int> col_idx;
  vector<double> values;
};

AdjacencyList read_adjacency_list(const string &path, bool weighted);

CSRGraph convert_to_csr(const AdjacencyList &list);

#endif