#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <limits>
#include "../src/floyd_warshall.hpp"

using namespace std;

namespace fs = filesystem;

void print_result(const FloydWarshallResult &res)
{
  cout << "Algorithm: Floyd-Warshall\n";

  if (res.has_negative_cycle)
  {
    cout << "Negative cycle: true\n";
  }
  else
  {
    cout << "Distance matrix:\n";

    for (int i = 0; i < res.V; ++i)
    {
      for (int j = 0; j < res.V; ++j)
      {
        if (res.dist_matrix[i][j] == numeric_limits<double>::infinity())
        {
          cout << "INF";
        }
        else
        {
          cout << static_cast<long long>(res.dist_matrix[i][j]);
        }
        cout << (j == res.V - 1 ? "" : " ");
      }
      cout << "\n";
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
    int V = 0;
    // load matrix (NOT TIMED)
    auto matrix = read_adjacency_matrix(path, V);

    // run algorithm
    FloydWarshallResult res = run_floyd_warshall(matrix, V);

    // Print output
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
      if (entry.path().extension() == ".txt" && entry.path().filename().string().rfind("fw_", 0) == 0)
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