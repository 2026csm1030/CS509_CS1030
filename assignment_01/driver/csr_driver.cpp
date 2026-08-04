// CSR driver.
//
// Usage:
//   csr_driver <test_file.txt> [--weighted]
//   csr_driver --all <tests_directory> [--weighted]
//
// This driver's job is to demonstrate and time the adjacency-list -> CSR
// conversion as a deliverable in its own right (per the assignment's
// "CSR graph implementation" requirement under the individual task).
//
// Timing rule: reading/parsing the adjacency-list file happens BEFORE the
// timer starts. The timer wraps only the convert_to_csr() call — that
// conversion is exactly what's being measured here.
//
// Note: when CSR is later used as input to a graph algorithm (BFS/DFS/SSSP
// in the buddy assignment), THIS conversion time must NOT be folded into
// that algorithm's reported time — conversion is preprocessing there. Here,
// conversion is the thing under test, so it's timed on its own.

#include "../src/csr.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;
using Clock = std::chrono::steady_clock;

// Prints CSR arrays. For large graphs (large V/E) this would be extremely
// verbose, so full arrays are only printed below a threshold; otherwise a
// summary + a small preview is shown.
static void print_csr_summary(const CSRGraph &csr, bool weighted)
{
  std::cout << "CSR arrays:\n";
  std::cout << "  V = " << csr.V << ", total entries (E, directed) = " << csr.E << "\n";

  const size_t PREVIEW_LIMIT = 30;
  bool truncate = csr.row_ptr.size() > PREVIEW_LIMIT || csr.col_idx.size() > PREVIEW_LIMIT;

  auto print_int_vec = [&](const std::string &label, const std::vector<int> &v)
  {
    std::cout << "  " << label << " (" << v.size() << " entries): ";
    size_t limit = truncate ? std::min(v.size(), PREVIEW_LIMIT) : v.size();
    for (size_t i = 0; i < limit; ++i)
    {
      std::cout << v[i];
      if (i + 1 < limit)
        std::cout << ' ';
    }
    if (truncate && v.size() > limit)
      std::cout << " ... (truncated)";
    std::cout << "\n";
  };

  print_int_vec("row_ptr", csr.row_ptr);
  print_int_vec("col_idx", csr.col_idx);

  if (weighted)
  {
    std::cout << "  values  (" << csr.values.size() << " entries): ";
    size_t limit = truncate ? std::min(csr.values.size(), PREVIEW_LIMIT) : csr.values.size();
    for (size_t i = 0; i < limit; ++i)
    {
      std::cout << csr.values[i];
      if (i + 1 < limit)
        std::cout << ' ';
    }
    if (truncate && csr.values.size() > limit)
      std::cout << " ... (truncated)";
    std::cout << "\n";
  }
}

static bool run_one_file(const std::string &path, bool weighted)
{
  std::cout << "==============================================\n";
  std::cout << "Test file: " << path << "\n";

  // --- File reading/parsing: NOT timed ---
  AdjacencyList list;
  try
  {
    list = read_adjacency_list(path, weighted);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error reading " << path << ": " << e.what() << "\n";
    return false;
  }
  std::cout << "Input: adjacency list, V = " << list.V << ", E = " << list.E
            << ", weighted = " << (weighted ? "yes" : "no") << "\n\n";

  // --- Conversion: THIS is the timed algorithm for this driver ---
  auto t0 = Clock::now();
  CSRGraph csr = convert_to_csr(list);
  auto t1 = Clock::now();
  double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

  std::cout << "Algorithm: Adjacency List -> CSR Conversion\n";
  print_csr_summary(csr, weighted);
  std::cout << "Execution time: " << std::fixed << std::setprecision(4) << ms << " ms\n\n";
  return true;
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage:\n"
              << "  " << argv[0] << " <test_file.txt> [--weighted]\n"
              << "  " << argv[0] << " --all <tests_directory> [--weighted]\n";
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
    bool weighted = (argc >= 4 && std::string(argv[3]) == "--weighted");

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
    std::string path = first_arg;
    bool weighted = (argc >= 3 && std::string(argv[2]) == "--weighted");

    if (!fs::exists(path))
    {
      std::cerr << "Error: input file not found: " << path << "\n";
      return 1;
    }
    return run_one_file(path, weighted) ? 0 : 1;
  }
}