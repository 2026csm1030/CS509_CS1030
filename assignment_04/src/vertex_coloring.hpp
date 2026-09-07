#ifndef VERTEX_COLORING_HPP
#define VERTEX_COLORING_HPP

#include "../../assignment_01/src/csr.hpp"
#include <vector>

struct VertexColoringResult
{
  std::vector<int> colors;        // Color assigned to each vertex (index = vertex ID)
  int colors_used = 0;            // Total number of unique colors used
  double execution_time_ms = 0.0; // Execution time in milliseconds
};

VertexColoringResult greedy_vertex_coloring(const CSRGraph &csr);

#endif