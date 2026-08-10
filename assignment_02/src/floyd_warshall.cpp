#include "floyd_warshall.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <limits>
#include <stdexcept>

using namespace std;

const double INF = numeric_limits<double>::infinity();

vector<vector<double>> read_adjacency_matrix(const string &path, int &V)
{
  ifstream fin(path);

  if (!fin.is_open())
  {
    throw runtime_error("Could not open file");
  }

  if (!(fin >> V) || V <= 0)
  {
    throw runtime_error("Invalid or missing vertex count V");
  }

  vector<vector<double>> matrix(V, vector<double>(V, INF));

  for (int i = 0; i < V; ++i)
  {
    for (int j = 0; j < V; ++j)
    {
      string token;

      if (!(fin >> token))
      {
        throw runtime_error("Malformed matrix entry at (" + to_string(i) + ", " + to_string(j) + ")");
      }

      if (token == "INF" || token == "inf")
      {
        matrix[i][j] = INF;
      }
      else
      {
        matrix[i][j] = stod(token);
      }
    }
  }

  return matrix;
}

FloydWarshallResult run_floyd_warshall(const vector<vector<double>> &adj_matrix, int V)
{
  FloydWarshallResult result;
  result.V = V;

  // start timer AFTER loading matrix into memory
  auto start_time = chrono::high_resolution_clock::now();

  vector<vector<double>> dist = adj_matrix;

  // DP loop: O(V^3)
  for (int k = 0; k < V; ++k)
  {
    for (int i = 0; i < V; ++i)
    {
      if (dist[i][k] == INF)
        continue;

      for (int j = 0; j < V; ++j)
      {
        if (dist[k][j] == INF)
          continue;

        if (dist[i][k] + dist[k][j] < dist[i][j])
        {
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }

  // check diagonal for negative-weight cycles: dist[i][i] < 0
  bool negative_cycle = false;

  for (int i = 0; i < V; ++i)
  {
    if (dist[i][i] < 0.0)
    {
      negative_cycle = true;
      break;
    }
  }

  auto stop_time = chrono::high_resolution_clock::now();

  chrono::duration<double, milli> duration = stop_time - start_time;

  result.execution_time_ms = duration.count();
  result.has_negative_cycle = negative_cycle;

  if (!negative_cycle)
  {
    result.dist_matrix = move(dist);
  }

  return result;
}