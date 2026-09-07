#include "../src/pagerank.hpp"
#include "../../assignment_01/src/csr.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <algorithm>

using namespace std;

namespace fs = filesystem;

void run_test_case(const string &path)
{
  cout << "\nProcessing file: " << path << "\n";

  PageRankParams params;

  try
  {
    params = read_pagerank_params(path);
  }
  catch (const exception &e)
  {
    cerr << "Error reading PageRank parameters: " << e.what() << "\n";
    return;
  }

  AdjacencyList adj_list;

  try
  {
    adj_list = read_adjacency_list(path, false);
  }
  catch (const exception &e)
  {
    cerr << "Error reading graph input: " << e.what() << "\n";
    return;
  }

  CSRGraph csr = convert_to_csr(adj_list);

  // Measure core algorithm execution time only
  auto start_time = chrono::high_resolution_clock::now();
  PageRankResult result = compute_pagerank(csr, params);
  auto end_time = chrono::high_resolution_clock::now();

  chrono::duration<double, milli> elapsed = end_time - start_time;

  result.execution_time_ms = elapsed.count();

  // Section 6.3 Expected PageRank Output format
  cout << "Algorithm: PageRank\n";
  // cout << fixed << setprecision(2) << "Damping: " << params.damping << "\n";
  cout << "Vertex ranks:\n";
  cout << fixed << setprecision(6);

  for (int u = 0; u < csr.V; ++u)
  {
    cout << u << " " << result.ranks[u] << "\n";
  }

  cout << "Sum of ranks: " << result.sum_of_ranks << "\n";
  cout << "Iterations: " << result.iterations << "\n";
  cout << "Converged: " << (result.converged ? "true" : "false") << "\n";

  // Log Top Vertex right before execution time
  cout << "Top Vertex: " << result.top_vertex << " (Rank: " << result.top_rank << ")\n";
  cout << fixed << setprecision(2) << "Damping: " << params.damping << "\n";
  cout << fixed << setprecision(4);
  cout << "Execution time: " << result.execution_time_ms << " ms\n";
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " <input_file_path_or_--all> [dir_path]\n";
    return 1;
  }

  string arg1 = argv[1];

  if (arg1 == "--all")
  {
    string dir_path = (argc >= 3) ? argv[2] : "assignment_04/tests";

    if (!fs::exists(dir_path))
    {
      cerr << "Error: Directory " << dir_path << " does not exist.\n";
      return 1;
    }

    vector<string> files;

    for (const auto &entry : fs::directory_iterator(dir_path))
    {
      if (entry.path().extension() == ".txt" && entry.path().filename().string().rfind("pagerank_", 0) == 0)
      {
        files.push_back(entry.path().string());
      }
    }

    sort(files.begin(), files.end());

    for (const auto &file : files)
    {
      run_test_case(file);
    }
  }
  else
  {
    run_test_case(arg1);
  }

  return 0;
}