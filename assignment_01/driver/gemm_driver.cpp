// GEMM driver.
//
// Usage:
//   gemm_driver <path/to/test_file.txt> [block_size]
//   gemm_driver --all <tests_directory> [block_size]

#include "../src/gemm.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

namespace fs = filesystem;
using Clock = chrono::steady_clock;

// Compares two matrices for equality (used to sanity-check simple vs
// blocking agree). Not timed; purely a correctness check for the driver.
static bool matrices_equal(const Matrix &a, const Matrix &b, double eps = 1e-6)
{
  if (a.rows != b.rows || a.cols != b.cols)
    return false;
  for (size_t i = 0; i < a.data.size(); ++i)
  {
    if (abs(a.data[i] - b.data[i]) > eps)
      return false;
  }
  return true;
}

static void run_one_file(const string &path, int block_size)
{
  cout << "==============================================\n";
  cout << "Test file: " << path << "\n";

  GemmInput in;
  try
  {
    in = read_gemm_input(path);
  }
  catch (const exception &e)
  {
    cerr << "Error reading " << path << ": " << e.what() << "\n";
    return;
  }

  cout << "Dimensions: A is " << in.M << "x" << in.K
       << ", B is " << in.K << "x" << in.N << "\n\n";

  auto t1_start = Clock::now();
  Matrix C_simple = gemm_simple(in.A, in.B);
  auto t1_end = Clock::now();
  double simple_ms = chrono::duration<double, milli>(t1_end - t1_start).count();

  cout << "Algorithm: GEMM Simple\n";
  cout << "Result matrix:\n";
  print_matrix(C_simple);
  cout << "Execution time: " << fixed << setprecision(4)
       << simple_ms << " ms\n\n";

  auto t2_start = Clock::now();
  Matrix C_blocking = gemm_blocking(in.A, in.B, block_size);
  auto t2_end = Clock::now();
  double blocking_ms = chrono::duration<double, milli>(t2_end - t2_start).count();

  cout << "Algorithm: GEMM Blocking (block size = " << block_size << ")\n";
  cout << "Result matrix:\n";
  print_matrix(C_blocking);
  cout << "Execution time: " << fixed << setprecision(4)
       << blocking_ms << " ms\n\n";

  if (matrices_equal(C_simple, C_blocking))
  {
    cout << "Status: PASS (simple and blocking results match)\n";
  }
  else
  {
    cout << "Status: FAIL (results differ)\n";
  }
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    cerr << "Usage:\n"
         << "  " << argv[0] << " <test_file.txt> [block_size]\n"
         << "  " << argv[0] << " --all <tests_directory> [block_size]\n";
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
    int block_size = (argc >= 4) ? stoi(argv[3]) : 32;

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

    for (const auto &f : files)
    {
      run_one_file(f.string(), block_size);
    }
  }
  else
  {
    string path = first_arg;
    int block_size = (argc >= 3) ? stoi(argv[2]) : 32;

    if (!fs::exists(path))
    {
      cerr << "Error: input file not found: " << path << "\n";
      return 1;
    }
    run_one_file(path, block_size);
  }

  return 0;
}