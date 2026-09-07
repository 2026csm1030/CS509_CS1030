#include "vertex_coloring.hpp"
#include <algorithm>
#include <vector>

using namespace std;

VertexColoringResult greedy_vertex_coloring(const CSRGraph &csr)
{
  VertexColoringResult result;

  // Initialize all vertices as uncolored (-1)
  result.colors.assign(csr.V, -1);

  if (csr.V == 0)
  {
    return result;
  }

  // Step 1: Compute degree of each vertex from CSR representation
  // Degree of vertex u is row_ptr[u+1] - row_ptr[u]

  vector<pair<int, int>> order(csr.V);

  for (int u = 0; u < csr.V; ++u)
  {
    int degree = csr.row_ptr[u + 1] - csr.row_ptr[u];
    order[u] = {degree, u};
  }

  // Step 2: Welsh-Powell Ordering — Sort vertices by non-increasing degree

  stable_sort(order.begin(), order.end(), [](const pair<int, int> &a, const pair<int, int> &b)
              {
                // Higher degree first
                return a.first > b.first; });

  // Step 3: Color vertices in Welsh-Powell order
  vector<bool> used_colors;

  int max_color = -1;

  for (int i = 0; i < csr.V; ++i)
  {
    int u = order[i].second;

    // Reset tracking vector for available colors

    fill(used_colors.begin(), used_colors.end(), false);

    // Check assigned colors of all adjacent neighbors

    int start = csr.row_ptr[u];
    int end = csr.row_ptr[u + 1];

    for (int e = start; e < end; ++e)
    {
      int neighbor = csr.col_idx[e];
      int c = result.colors[neighbor];

      if (c != -1)
      {
        if (c >= static_cast<int>(used_colors.size()))
        {
          used_colors.resize(c + 1, false);
        }

        // Mark neighbor's color as unavailable
        used_colors[c] = true;
      }
    }

    // Assign smallest available non-conflicting color index
    int assigned_color = 0;

    while (assigned_color < static_cast<int>(used_colors.size()) && used_colors[assigned_color])
    {
      assigned_color++;
    }

    result.colors[u] = assigned_color;
    max_color = max(max_color, assigned_color);
  }

  result.colors_used = (csr.V > 0) ? (max_color + 1) : 0;
  return result;
}