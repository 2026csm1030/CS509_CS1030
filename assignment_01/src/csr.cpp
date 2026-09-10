#include "csr.hpp"

using namespace std;

AdjacencyList read_adjacency_list(const string &path, bool weighted)
{
  ifstream file(path);

  if (!file.is_open())
  {
    throw runtime_error("Could not open input file: " + path);
  }

  AdjacencyList list;
  list.weighted = weighted;

  if (!(file >> list.V >> list.E))
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

    if (!(file >> u >> degree))
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
        if (!(file >> neighbor >> w))
        {
          throw runtime_error("Malformed weighted edge in " + path);
        }
        if (w <= 0.0)
        {
          throw runtime_error("Non-positive edge weight in " + path);
        }
      }
      else
      {
        if (!(file >> neighbor))
        {
          throw runtime_error("Malformed edge in " + path);
        }
      }

      list.adj[u].push_back({neighbor, w});
    }
  }

  string tag;

  // if (fin >> tag)

  // {
  //   if (tag != "SOURCE")
  //   {
  //     throw runtime_error("Expected SOURCE tag in " + path);
  //   }
  //   if (!(fin >> list.source))
  //   {
  //     throw runtime_error("Missing source vertex in " + path);
  //   }
  // }

  if (file >> tag)
  {
    if (tag == "SOURCE")
    {
      if (!(file >> list.source))
      {
        throw runtime_error("Missing source vertex in " + path);
      }
    }
    // If the tag is DAMPING, TOLERANCE, or anything else (Assignment 4),
    // safely ignore it here so PageRank/Vertex Coloring parsers can handle it.
  }

  return list;
}

// Adjacency-list -> CSR conversion.

CSRGraph convert_to_csr(const AdjacencyList &list)
{
  CSRGraph csr;

  // Number of vertices stays the same
  csr.V = list.V;

  // ----------------------------------------
  // Step 1: Calculate row_ptr
  // ----------------------------------------

  csr.row_ptr.resize(csr.V + 1);

  csr.row_ptr[0] = 0;

  for (int u = 0; u < csr.V; u++)
  {
    csr.row_ptr[u + 1] =
        csr.row_ptr[u] + list.adj[u].size();
  }

  // Total number of edges
  csr.E = csr.row_ptr[csr.V];

  // ----------------------------------------
  // Step 2: Create col_idx (edges)
  // ----------------------------------------

  csr.col_idx.resize(csr.E);

  int position = 0;

  for (int u = 0; u < csr.V; u++)
  {
    for (const Edge &e : list.adj[u])
    {
      csr.col_idx[position] = e.to;
      position++;
    }
  }

  // ----------------------------------------
  // Step 3: Store weights if graph is weighted
  // ----------------------------------------

  if (list.weighted)
  {
    csr.values.resize(csr.E);

    position = 0;

    for (int u = 0; u < csr.V; u++)
    {
      for (const Edge &e : list.adj[u])
      {
        csr.values[position] = e.weight;
        position++;
      }
    }
  }

  return csr;
}