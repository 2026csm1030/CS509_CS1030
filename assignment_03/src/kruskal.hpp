#ifndef KRUSKAL_HPP
#define KRUSKAL_HPP

#include <vector>
#include <iostream>
#include "../../assignment_01/src/csr.hpp"

using namespace std;

struct MSTEdge
{
  int u;
  int v;
  double weight;
};

struct MSTResult
{
  vector<MSTEdge> edges;
  double total_weight = 0.0;
  double execution_time_ms = 0.0;
};

// Disjoint Set Union (DSU) with Path Compression and Union by Rank
struct DSU
{
  vector<int> parent;
  vector<int> rank;

  DSU(int n)
  {
    parent.resize(n);
    rank.assign(n, 0);

    for (int i = 0; i < n; ++i)
    {
      parent[i] = i;
    }
  }

  int find(int i)
  {
    if (parent[i] == i)
    {
      return i;
    }

    return parent[i] = find(parent[i]);
  }

  bool unite(int i, int j)
  {
    int root_i = find(i);
    int root_j = find(j);

    if (root_i != root_j)
    {
      if (rank[root_i] < rank[root_j])
      {
        swap(root_i, root_j);
      }

      parent[root_j] = root_i;

      if (rank[root_i] == rank[root_j])
      {
        rank[root_i]++;
      }

      return true;
    }

    return false;
  }
};

MSTResult run_kruskal(const CSRGraph &graph);

void print_kruskal_result(const MSTResult &res);

#endif