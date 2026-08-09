#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <string>
#include "../src/csr.hpp"
#include "../src/bellman_ford.hpp"

using namespace std;

namespace fs = filesystem;

void print_result(const BellmanFordResult &res)
{
  cout << "Algorithm: Bellman-Ford\n";
  cout << "Source: " << res.source << "\n";

  if (res.has_negative_cycle)
  {
    cout << "Negative cycle: true\n";
  }
  else
  {
    cout << "Vertex Distance\n";
    for (size_t i = 0; i < res.distances.size(); ++i)
    {
      if (res.distances[i] == numeric_limits<double>::infinity())
      {
        cout << i << " INF\n";
      }
      else
      {
        cout << i << " " << static_cast<long long>(res.distances[i]) << "\n";
      }
    }
    cout << "Negative cycle: none\n";
  }
  cout << fixed << setprecision(3);
  cout << "Execution time: " << res.execution_time_ms << " ms\n";
}

void process_single_file(const string &path)
{
  cout << "\nProcessing file: " << path << "\n";
  try
  {
    // 1. read input adjacency list
    AdjacencyList adj_list = read_adjacency_list(path, true);

    // 2. convert to CSR structure (NOT TIMED)
    CSRGraph csr = convert_to_csr(adj_list);

    // 3. run Bellman-Ford
    BellmanFordResult res = run_bellman_ford(csr, adj_list.source);

    // 4. print result
    print_result(res);
  }
  catch (const exception &e)
  {
    cerr << "Error: " << e.what() << "\n";
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " <test_file_path> OR " << argv[0] << " --all <test_dir>\n";
    return 1;
  }

  string arg1 = argv[1];

  if (arg1 == "--all")
  {
    if (argc < 3)
    {
      cerr << "Error: Missing directory path for --all\n";
      return 1;
    }

    string dir_path = argv[2];

    for (const auto &entry : fs::directory_iterator(dir_path))
    {
      if (entry.path().extension() == ".txt" && entry.path().filename().string().rfind("bf_", 0) == 0)
      {
        process_single_file(entry.path().string());
      }
    }
  }
  else
  {
    process_single_file(arg1);
  }

  return 0;
}