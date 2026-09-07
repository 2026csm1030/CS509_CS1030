#include "pagerank.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <stdexcept>

using namespace std;

PageRankParams read_pagerank_params(const string &path)
{
  ifstream fin(path);

  if (!fin.is_open())
  {
    throw runtime_error("Could not open file for parameters: " + path);
  }

  PageRankParams params;

  string line;

  while (getline(fin, line))
  {
    istringstream iss(line);

    string key;

    if (iss >> key)
    {
      if (key == "DAMPING")
      {
        iss >> params.damping;
      }
      else if (key == "TOLERANCE")
      {
        iss >> params.tolerance;
      }
      else if (key == "MAX_ITERATIONS")
      {
        iss >> params.max_iterations;
      }
    }
  }

  if (params.damping <= 0.0 || params.damping >= 1.0)
  {
    throw runtime_error("Invalid damping factor: must be between 0 and 1.");
  }
  if (params.tolerance <= 0.0)
  {
    throw runtime_error("Invalid tolerance: must be > 0.");
  }
  if (params.max_iterations <= 0)
  {
    throw runtime_error("Invalid max_iterations: must be > 0.");
  }

  return params;
}

PageRankResult compute_pagerank(const CSRGraph &csr, const PageRankParams &params)
{
  PageRankResult result;

  int N = csr.V;

  if (N == 0)
  {
    result.converged = true;
    return result;
  }

  // Compute outdegrees

  vector<int> outdegree(N, 0);

  for (int u = 0; u < N; ++u)
  {
    outdegree[u] = csr.row_ptr[u + 1] - csr.row_ptr[u];
  }

  // Initialize all ranks to 1/N
  vector<double> pr(N, 1.0 / N);
  vector<double> new_pr(N, 0.0);

  double d = params.damping;
  double base_rank = (1.0 - d) / N;

  int iter = 0;
  bool converged = false;

  while (iter < params.max_iterations)
  {
    iter++;

    double dangling_sum = 0.0;

    for (int u = 0; u < N; ++u)
    {
      if (outdegree[u] == 0)
      {
        dangling_sum += pr[u];
      }
    }

    double dangling_contribution = d * (dangling_sum / N);

    for (int i = 0; i < N; ++i)
    {
      new_pr[i] = base_rank + dangling_contribution;
    }

    for (int u = 0; u < N; ++u)
    {
      if (outdegree[u] > 0)
      {
        double contribution = d * (pr[u] / outdegree[u]);
        int start = csr.row_ptr[u];
        int end = csr.row_ptr[u + 1];

        for (int e = start; e < end; ++e)
        {
          int v = csr.col_idx[e];
          new_pr[v] += contribution;
        }
      }
    }

    double total_change = 0.0;

    for (int i = 0; i < N; ++i)
    {
      total_change += abs(new_pr[i] - pr[i]);
    }

    pr = new_pr;

    if (total_change <= params.tolerance)
    {
      converged = true;
      break;
    }
  }

  result.ranks = pr;
  result.iterations = iter;
  result.converged = converged;

  // Calculate sum of ranks and locate the Top Vertex
  result.sum_of_ranks = 0.0;
  result.top_vertex = 0;
  result.top_rank = pr[0];

  for (int u = 0; u < N; ++u)
  {
    result.sum_of_ranks += pr[u];

    if (pr[u] > result.top_rank)
    {
      result.top_rank = pr[u];
      result.top_vertex = u;
    }
  }

  return result;
}