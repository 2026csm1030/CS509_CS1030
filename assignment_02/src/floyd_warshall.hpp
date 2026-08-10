#ifndef FLOYD_WARSHALL_HPP
#define FLOYD_WARSHALL_HPP

#include <vector>
#include <string>

using namespace std;

struct FloydWarshallResult
{
  int V = 0;
  bool has_negative_cycle = false;
  vector<vector<double>> dist_matrix;
  double execution_time_ms = 0.0;
};

// reads the dense V x V matrix
vector<vector<double>> read_adjacency_matrix(const string &path, int &V);

// runs all-pairs shortest path using Floyd-Warshall DP algorithm
FloydWarshallResult run_floyd_warshall(const vector<vector<double>> &adj_matrix, int V);

#endif