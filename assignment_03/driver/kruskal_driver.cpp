#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include "../../assignment_01/src/csr.hpp"
#include "../src/kruskal.hpp"

using namespace std;

namespace fs = filesystem;

void run_file(const string &path)
{
  cout << "\n=======================================================\n";
  cout << "Processing Test File: " << path << "\n";
  cout << "=======================================================\n";

  try
  {
    // reading adjacency list & converting to CSR

    AdjacencyList adj = read_adjacency_list(path, true);

    CSRGraph csr = convert_to_csr(adj);

    MSTResult result = run_kruskal(csr);

    print_kruskal_result(result);
  }
  catch (const exception &e)
  {
    cerr << "Error running test " << path << ": " << e.what() << "\n";
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cerr << "Usage: " << argv[0] << " <path_to_test_file>\n";
    cerr << "   or: " << argv[0] << " --all <test_directory_path>\n";
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

    if (!fs::exists(dir_path) || !fs::is_directory(dir_path))
    {
      cerr << "Error: Directory does not exist" << "\n";
      return 1;
    }

    vector<string> test_files;

    for (const auto &entry : fs::directory_iterator(dir_path))
    {
      if (entry.path().extension() == ".txt")
      {
        test_files.push_back(entry.path().string());
      }
    }

    sort(test_files.begin(), test_files.end());

    if (test_files.empty())
    {
      cout << "No .txt test files found" << "\n";
      return 0;
    }

    for (const auto &file : test_files)
    {
      run_file(file);
    }
  }
  else
  {
    run_file(arg1);
  }

  return 0;
}