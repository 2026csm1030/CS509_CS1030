#ifndef BELLMAN_FORD_HPP
#define BELLMAN_FORD_HPP

#include <vector>
#include <limits>
#include "csr.hpp"

using namespace std;

struct BellmanFordResult
{
  int source;
  bool has_negative_cycle = false;
  vector<double> distances;
  double execution_time_ms = 0.0;
};

// computes SSSP using CSR representation
BellmanFordResult run_bellman_ford(const CSRGraph &csr, int source);

#endif