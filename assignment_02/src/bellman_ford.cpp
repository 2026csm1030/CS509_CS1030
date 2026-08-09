#include "bellman_ford.hpp"
#include <chrono>
#include <limits>

using namespace std;
using namespace chrono;

BellmanFordResult run_bellman_ford(const CSRGraph &csr, int source)
{
  BellmanFordResult result;
  result.source = source;

  const double INF = numeric_limits<double>::infinity();
  vector<double> dist(csr.V, INF);

  // timing starts after CSR representation
  auto start_time = high_resolution_clock::now();

  if (source >= 0 && source < csr.V)
  {
    dist[source] = 0.0;
  }

  // step 1: relax all edges V - 1 times
  for (int i = 0; i < csr.V - 1; ++i)
  {
    bool updated = false;

    for (int u = 0; u < csr.V; ++u)
    {
      if (dist[u] == INF)
      {
        continue;
      }

      int row_start = csr.row_ptr[u];
      int row_end = csr.row_ptr[u + 1];

      for (int idx = row_start; idx < row_end; ++idx)
      {
        int v = csr.col_idx[idx];
        double weight = csr.values[idx];

        if (dist[u] + weight < dist[v])
        {
          dist[v] = dist[u] + weight;
          updated = true;
        }
      }
    }

    // early exit optimization if no distance changed in a full pass
    if (!updated)
    {
      break;
    }
  }

  // step 2: V-th pass for negative-weight cycle detection
  bool negative_cycle = false;

  for (int u = 0; u < csr.V; ++u)
  {
    if (dist[u] == INF)
    {
      continue;
    }

    int row_start = csr.row_ptr[u];
    int row_end = csr.row_ptr[u + 1];

    for (int idx = row_start; idx < row_end; ++idx)
    {
      int v = csr.col_idx[idx];
      double weight = csr.values[idx];

      if (dist[u] + weight < dist[v])
      {
        negative_cycle = true;
        break;
      }
    }

    if (negative_cycle)
      break;
  }

  auto stop_time = high_resolution_clock::now();
  duration<double, milli> duration = stop_time - start_time;

  result.execution_time_ms = duration.count();
  result.has_negative_cycle = negative_cycle;

  if (!negative_cycle)
  {
    result.distances = move(dist);
  }

  return result;
}