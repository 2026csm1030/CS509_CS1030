// GEMM driver.
//
// Usage:
//   gemm_driver <path/to/test_file.txt> [block_size]
//   gemm_driver --all <tests_directory> [block_size]
//
// For a single file: reads it, runs GEMM Simple and GEMM Blocking, prints
// both result matrices and their execution times.
// For --all: runs every *.txt file found directly inside the given directory.
//
// Timing rule (per assignment spec): the timer starts immediately before the
// algorithm call and stops immediately after. File reading/parsing and
// result printing are NOT included in the measured time.

#include "../src/gemm.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace fs = std::filesystem;
using Clock = std::chrono::steady_clock;

// Compares two matrices for equality (used to sanity-check simple vs
// blocking agree). Not timed; purely a correctness check for the driver.
static bool matrices_equal(const Matrix &a, const Matrix &b, double eps = 1e-6)
{
  if (a.rows != b.rows || a.cols != b.cols)
    return false;
  for (size_t i = 0; i < a.data.size(); ++i)
  {
    if (std::abs(a.data[i] - b.data[i]) > eps)
      return false;
  }
  return true;
}

static void run_one_file(const std::string &path, int block_size)
{
  std::cout << "==============================================\n";
  std::cout << "Test file: " << path << "\n";

  // --- Setup: NOT timed ---
  GemmInput in;
  try
  {
    in = read_gemm_input(path);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error reading " << path << ": " << e.what() << "\n";
    return;
  }
  std::cout << "Dimensions: A is " << in.M << "x" << in.K
            << ", B is " << in.K << "x" << in.N << "\n\n";

  // --- GEMM Simple: only this call is timed ---
  auto t1_start = Clock::now();
  Matrix C_simple = gemm_simple(in.A, in.B);
  auto t1_end = Clock::now();
  double simple_ms = std::chrono::duration<double, std::milli>(t1_end - t1_start).count();

  std::cout << "Algorithm: GEMM Simple\n";
  std::cout << "Result matrix:\n";
  print_matrix(C_simple);
  std::cout << "Execution time: " << std::fixed << std::setprecision(4)
            << simple_ms << " ms\n\n";

  // --- GEMM Blocking: only this call is timed ---
  auto t2_start = Clock::now();
  Matrix C_blocking = gemm_blocking(in.A, in.B, block_size);
  auto t2_end = Clock::now();
  double blocking_ms = std::chrono::duration<double, std::milli>(t2_end - t2_start).count();

  std::cout << "Algorithm: GEMM Blocking (block size = " << block_size << ")\n";
  std::cout << "Result matrix:\n";
  print_matrix(C_blocking);
  std::cout << "Execution time: " << std::fixed << std::setprecision(4)
            << blocking_ms << " ms\n\n";

  // --- Correctness check: NOT timed, just a driver-level sanity check ---
  if (matrices_equal(C_simple, C_blocking))
  {
    std::cout << "Status: PASS (simple and blocking results match)\n";
  }
  else
  {
    std::cout << "Status: FAIL (results differ)\n";
  }
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage:\n"
              << "  " << argv[0] << " <test_file.txt> [block_size]\n"
              << "  " << argv[0] << " --all <tests_directory> [block_size]\n";
    return 1;
  }

  std::string first_arg = argv[1];

  if (first_arg == "--all")
  {
    if (argc < 3)
    {
      std::cerr << "Error: --all requires a directory path.\n";
      return 1;
    }
    std::string dir = argv[2];
    int block_size = (argc >= 4) ? std::stoi(argv[3]) : 32;

    if (!fs::exists(dir) || !fs::is_directory(dir))
    {
      std::cerr << "Error: directory not found: " << dir << "\n";
      return 1;
    }

    std::vector<fs::path> files;
    for (const auto &entry : fs::directory_iterator(dir))
    {
      if (entry.path().extension() == ".txt")
        files.push_back(entry.path());
    }
    std::sort(files.begin(), files.end());

    if (files.empty())
    {
      std::cerr << "No .txt test files found in " << dir << "\n";
      return 1;
    }

    for (const auto &f : files)
    {
      run_one_file(f.string(), block_size);
    }
  }
  else
  {
    std::string path = first_arg;
    int block_size = (argc >= 3) ? std::stoi(argv[2]) : 32;

    if (!fs::exists(path))
    {
      std::cerr << "Error: input file not found: " << path << "\n";
      return 1;
    }
    run_one_file(path, block_size);
  }

  return 0;
}