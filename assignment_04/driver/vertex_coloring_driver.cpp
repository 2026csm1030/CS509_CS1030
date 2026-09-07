#include "../src/vertex_coloring.hpp"
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

  // Step 1: Parse input file to Adjacency List (Unweighted)
  AdjacencyList adj_list;

  try
  {
    adj_list = read_adjacency_list(path, false);
  }
  catch (const exception &e)
  {
    cerr << "Error reading input file: " << e.what() << "\n";
    return;
  }

  // Step 2: Convert to CSR representation (Outside timing window)
  CSRGraph csr = convert_to_csr(adj_list);

  // Step 3: Run algorithm and measure algorithm execution time only
  auto start_time = chrono::high_resolution_clock::now();

  VertexColoringResult result = greedy_vertex_coloring(csr);

  auto end_time = chrono::high_resolution_clock::now();

  chrono::duration<double, milli> elapsed = end_time - start_time;
  result.execution_time_ms = elapsed.count();

  // Step 4: Output results according to Assignment 4 specification
  cout << "Algorithm: Greedy Vertex Coloring\n";
  cout << "Vertex colors:\n";

  for (int u = 0; u < csr.V; ++u)
  {
    cout << u << " " << result.colors[u] << "\n";
  }

  cout << "Colors used: " << result.colors_used << "\n";
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
      if (entry.path().extension() == ".txt" && entry.path().filename().string().rfind("color_", 0) == 0)
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