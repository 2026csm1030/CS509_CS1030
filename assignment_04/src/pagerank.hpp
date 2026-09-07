#ifndef PAGERANK_HPP
#define PAGERANK_HPP

#include "../../assignment_01/src/csr.hpp"
#include <vector>
#include <string>

using namespace std;

struct PageRankParams
{
  double damping = 0.85;
  double tolerance = 0.0001;
  int max_iterations = 100;
};

struct PageRankResult
{
  vector<double> ranks;      // Final rank of each vertex
  double sum_of_ranks = 0.0; // Sum of all ranks (should be ~1.0)
  int iterations = 0;        // Total iterations performed
  bool converged = false;    // Whether change <= tolerance
  int top_vertex = -1;       // Vertex ID with the highest PageRank score
  double top_rank = 0.0;     // The highest PageRank score
  double execution_time_ms = 0.0;
};

// Reads PageRank metadata (DAMPING, TOLERANCE, MAX_ITERATIONS) from the input file
PageRankParams read_pagerank_params(const string &path);

// Core algorithm function
PageRankResult compute_pagerank(const CSRGraph &csr, const PageRankParams &params);

#endif