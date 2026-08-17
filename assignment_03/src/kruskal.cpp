#include "kruskal.hpp"
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;

MSTResult run_kruskal(const CSRGraph &csr)
{
  MSTResult result;

  if (csr.V <= 1)
  {
    return result;
  }

  auto start_time = chrono::high_resolution_clock::now();

  vector<MSTEdge> edges_all;
  edges_all.reserve(csr.E);

  // extract unique undirected edges from CSR representation

  for (int u = 0; u < csr.V; ++u)
  {
    int start_idx = csr.row_ptr[u];
    int end_idx = csr.row_ptr[u + 1];

    for (int i = start_idx; i < end_idx; ++i)
    {
      int v = csr.col_idx[i];

      double weight = csr.values.empty() ? 1.0 : csr.values[i];

      if (u < v)
      {
        edges_all.push_back({u, v, weight});
      }
    }
  }

  // sort edges in non-decreasing order of weight

  sort(edges_all.begin(), edges_all.end(), [](const MSTEdge &a, const MSTEdge &b)
       {
        if (a.weight != b.weight) {
          return a.weight < b.weight;
        }

        if (a.u != b.u) {
          return a.u < b.u;
        }
        
        return a.v < b.v; });

  DSU dsu(csr.V);

  int selected_edges = 0;

  for (const auto &edge : edges_all)
  {
    if (dsu.unite(edge.u, edge.v))
    {
      result.edges.push_back(edge);

      result.total_weight += edge.weight;

      selected_edges++;

      if (selected_edges == csr.V - 1)
      {
        break;
      }
    }
  }

  auto end_time = chrono::high_resolution_clock::now();

  chrono::duration<double, milli> duration = end_time - start_time;

  result.execution_time_ms = duration.count();

  return result;
}

void print_kruskal_result(const MSTResult &res)
{
  cout << "Algorithm: Kruskal's MST\n";
  cout << "MST edges:\n";

  for (const auto &e : res.edges)
  {
    cout << e.u << " " << e.v << " " << static_cast<long long>(e.weight) << "\n";
  }

  cout << "Total MST weight: " << static_cast<long long>(res.total_weight) << "\n";

  cout << fixed << setprecision(4);

  cout << "Execution time: " << res.execution_time_ms << " ms\n";
}