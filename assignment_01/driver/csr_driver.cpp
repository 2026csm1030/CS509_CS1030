// CSR driver.
//
// Usage:
//   csr_driver <test_file.txt> [--weighted]
//   csr_driver --all <tests_directory> [--weighted]

#include "../src/csr.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <algorithm>

using namespace std;

namespace fs = filesystem;
using Clock = chrono::steady_clock;

static void print_csr_summary(const CSRGraph &csr, bool weighted)
{
  cout << "CSR arrays:\n";
  cout << "  V = " << csr.V << ", total entries (E, directed) = " << csr.E << "\n";

  const size_t PREVIEW_LIMIT = 30;
  bool truncate = csr.row_ptr.size() > PREVIEW_LIMIT || csr.col_idx.size() > PREVIEW_LIMIT;

  auto print_int_vec = [&](const string &label, const vector<int> &v)
  {
    cout << "  " << label << " (" << v.size() << " entries): ";
    size_t limit = truncate ? min(v.size(), PREVIEW_LIMIT) : v.size();

    for (size_t i = 0; i < limit; ++i)
    {
      cout << v[i];
      if (i + 1 < limit)
        cout << ' ';
    }

    if (truncate && v.size() > limit)
    {
      cout << " ... (truncated)";
    }

    cout << "\n";
  };

  print_int_vec("row_ptr", csr.row_ptr);
  print_int_vec("col_idx", csr.col_idx);

  if (weighted)
  {
    cout << "  values  (" << csr.values.size() << " entries): ";
    size_t limit = truncate ? min(csr.values.size(), PREVIEW_LIMIT) : csr.values.size();

    for (size_t i = 0; i < limit; ++i)
    {
      cout << csr.values[i];
      if (i + 1 < limit)
        cout << ' ';
    }

    if (truncate && csr.values.size() > limit)
    {
      cout << " ... (truncated)";
    }

    cout << "\n";
  }
}

static bool run_one_file(const string &path, bool weighted)
{
  cout << "==============================================\n";
  cout << "Test file: " << path << "\n";

  AdjacencyList list;
  try
  {
    list = read_adjacency_list(path, weighted);
  }
  catch (const exception &e)
  {
    cerr << "Error reading " << path << ": " << e.what() << "\n";
    return false;
  }

  cout << "Input: adjacency list, V = " << list.V << ", E = " << list.E
       << ", weighted = " << (weighted ? "yes" : "no") << "\n\n";

  auto t0 = Clock::now();
  CSRGraph csr = convert_to_csr(list);
  auto t1 = Clock::now();
  double ms = chrono::duration<double, milli>(t1 - t0).count();

  cout << "Algorithm: Adjacency List -> CSR Conversion\n";
  print_csr_summary(csr, weighted);
  cout << "Execution time: " << fixed << setprecision(4) << ms << " ms\n\n";

  return true;
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    cerr << "Usage:\n"
         << "  " << argv[0] << " <test_file.txt> [--weighted]\n"
         << "  " << argv[0] << " --all <tests_directory> [--weighted]\n";

    return 1;
  }

  string first_arg = argv[1];

  if (first_arg == "--all")
  {
    if (argc < 3)
    {
      cerr << "Error: --all requires a directory path.\n";
      return 1;
    }

    string dir = argv[2];
    bool weighted = (argc >= 4 && string(argv[3]) == "--weighted");

    if (!fs::exists(dir) || !fs::is_directory(dir))
    {
      cerr << "Error: directory not found: " << dir << "\n";
      return 1;
    }

    vector<fs::path> files;

    for (const auto &entry : fs::directory_iterator(dir))
    {
      if (entry.path().extension() == ".txt")
        files.push_back(entry.path());
    }

    sort(files.begin(), files.end());

    if (files.empty())
    {
      cerr << "No .txt test files found in " << dir << "\n";
      return 1;
    }

    bool all_ok = true;

    for (const auto &f : files)
    {
      if (!run_one_file(f.string(), weighted))
        all_ok = false;
    }

    return all_ok ? 0 : 1;
  }
  else
  {
    string path = first_arg;
    bool weighted = (argc >= 3 && string(argv[2]) == "--weighted");

    if (!fs::exists(path))
    {
      cerr << "Error: input file not found: " << path << "\n";
      return 1;
    }
    return run_one_file(path, weighted) ? 0 : 1;
  }
}