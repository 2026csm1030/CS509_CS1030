#include "prim.hpp"
#include <queue>
#include <chrono>
#include <iomanip>

using namespace std;

struct PQElement
{
  double weight;
  int u;
  int v;

  // min-heap ordering: lowest weight first

  bool operator>(const PQElement &other) const
  {
    if (weight != other.weight)
    {
      return weight > other.weight;
    }

    if (u != other.u)
    {
      return u > other.u;
    }

    return v > other.v;
  }
};

MSTResult run_prim(const CSRGraph &csr, int start_node)
{
  MSTResult result;

  if (csr.V <= 1)
  {
    return result;
  }

  auto start_time = chrono::high_resolution_clock::now();

  vector<bool> in_mst(csr.V, false);

  priority_queue<PQElement, vector<PQElement>, greater<PQElement>> pq;

  // start growing tree from start_node (vertex 0)

  in_mst[start_node] = true;

  // push all outgoing edges

  auto push_outgoing_edges = [&](int curr)
  {
    int start_idx = csr.row_ptr[curr];

    int end_idx = csr.row_ptr[curr + 1];

    for (int i = start_idx; i < end_idx; ++i)
    {
      int neighbor = csr.col_idx[i];

      if (!in_mst[neighbor])
      {
        double weight = csr.values.empty() ? 1.0 : csr.values[i];
        pq.push({weight, curr, neighbor});
      }
    }
  };

  push_outgoing_edges(start_node);

  int edges_selected = 0;

  while (!pq.empty() && edges_selected < csr.V - 1)
  {
    PQElement edge = pq.top();

    pq.pop();

    int u = edge.u;
    int v = edge.v;
    double w = edge.weight;

    // skip if neighbor v is already part of the tree

    if (in_mst[v])
    {
      continue;
    }

    // add edge (u, v) to MST

    in_mst[v] = true;

    result.edges.push_back({u, v, w});

    result.total_weight += w;
    edges_selected++;

    // add new reachable edges from v

    push_outgoing_edges(v);
  }

  auto end_time = chrono::high_resolution_clock::now();

  chrono::duration<double, milli> duration = end_time - start_time;

  result.execution_time_ms = duration.count();

  return result;
}

void print_prim_result(const MSTResult &res)
{
  cout << "Algorithm: Prim's MST\n";
  cout << "MST edges:\n";

  for (const auto &e : res.edges)
  {
    cout << e.u << " " << e.v << " " << static_cast<long long>(e.weight) << "\n";
  }

  cout << "Total MST weight: " << static_cast<long long>(res.total_weight) << "\n";

  cout << fixed << setprecision(4);

  cout << "Execution time: " << res.execution_time_ms << " ms\n";
}