#include "csr.hpp"

using namespace std;

AdjacencyList read_adjacency_list(const string &path, bool weighted)
{
  ifstream fin(path);
  if (!fin.is_open())
  {
    throw runtime_error("Could not open input file: " + path);
  }

  AdjacencyList list;
  list.weighted = weighted;

  if (!(fin >> list.V >> list.E))
  {
    throw runtime_error("Malformed header at" + path);
  }
  if (list.V < 0)
  {
    throw runtime_error("Invalid vertex count in " + path);
  }

  list.adj.resize(list.V);

  for (int i = 0; i < list.V; ++i)
  {
    int u, degree;

    if (!(fin >> u >> degree))
    {
      throw runtime_error("Malformed adjacency row " + to_string(i) + " in " + path);
    }

    if (u < 0 || u >= list.V)
    {
      throw runtime_error("Vertex id out of range in " + path);
    }

    list.adj[u].reserve(degree);

    for (int d = 0; d < degree; ++d)
    {
      int neighbor;
      double w = 1.0;

      if (weighted)
      {
        if (!(fin >> neighbor >> w))
        {
          throw runtime_error("Malformed weighted edge in " + path);
        }

        // commented the non-positive weights
        // if (w <= 0.0)
        // {
        //   throw runtime_error("Non-positive edge weight in " + path);
        // }
      }
      else
      {
        if (!(fin >> neighbor))
        {
          throw runtime_error("Malformed edge in " + path);
        }
      }

      list.adj[u].push_back({neighbor, w});
    }
  }

  string tag;

  if (fin >> tag)

  {
    if (tag != "SOURCE")
    {
      throw runtime_error("Expected SOURCE tag in " + path);
    }
    if (!(fin >> list.source))
    {
      throw runtime_error("Missing source vertex in " + path);
    }
  }

  return list;
}

// Adjacency-list -> CSR conversion.

CSRGraph convert_to_csr(const AdjacencyList &list)
{
  CSRGraph csr;
  csr.V = list.V;

  csr.row_ptr.assign(list.V + 1, 0);

  for (int u = 0; u < list.V; ++u)
  {
    csr.row_ptr[u + 1] = csr.row_ptr[u] + static_cast<int>(list.adj[u].size());
  }

  int total_edges = csr.row_ptr[list.V];

  csr.E = total_edges;
  csr.col_idx.resize(total_edges);

  if (list.weighted)
  {
    csr.values.resize(total_edges);
  }

  vector<int> cursor(csr.row_ptr.begin(), csr.row_ptr.end() - 1);

  for (int u = 0; u < list.V; ++u)
  {
    for (const Edge &e : list.adj[u])
    {
      int pos = cursor[u]++;

      csr.col_idx[pos] = e.to;

      if (list.weighted)
        csr.values[pos] = e.weight;
    }
  }

  return csr;
}