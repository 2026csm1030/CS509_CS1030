#ifndef GEMM_HPP
#define GEMM_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

// Simple row-major matrix wrapper over a flat vector.
// Flat storage (instead of vector<vector<double>>) is used deliberately:
// it keeps rows contiguous in memory, which is what makes the blocking
// implementation's cache-reuse argument meaningful.
struct Matrix
{
  int rows = 0;
  int cols = 0;
  std::vector<double> data;

  Matrix() = default;
  Matrix(int r, int c) : rows(r), cols(c), data(static_cast<size_t>(r) * c, 0.0) {}

  double &at(int r, int c)
  {
    return data[static_cast<size_t>(r) * cols + c];
  }
  double at(int r, int c) const
  {
    return data[static_cast<size_t>(r) * cols + c];
  }
};

// Holds the full parsed GEMM test case: A (MxK), B (KxN).
struct GemmInput
{
  int M = 0, K = 0, N = 0;
  Matrix A;
  Matrix B;
};

// ---- File I/O (NOT timed) ----

// Reads a GEMM input file in the assignment's specified format:
//   M K N
//   A row 0 ... A row M-1
//   B row 0 ... B row K-1
inline GemmInput read_gemm_input(const std::string &path)
{
  std::ifstream fin(path);
  if (!fin.is_open())
  {
    throw std::runtime_error("Could not open input file: " + path);
  }

  GemmInput in;
  if (!(fin >> in.M >> in.K >> in.N))
  {
    throw std::runtime_error("Malformed header (expected: M K N) in " + path);
  }
  if (in.M <= 0 || in.K <= 0 || in.N <= 0)
  {
    throw std::runtime_error("Invalid dimensions in " + path);
  }

  in.A = Matrix(in.M, in.K);
  for (int i = 0; i < in.M; ++i)
    for (int j = 0; j < in.K; ++j)
    {
      if (!(fin >> in.A.at(i, j)))
        throw std::runtime_error("Malformed matrix A in " + path);
    }

  in.B = Matrix(in.K, in.N);
  for (int i = 0; i < in.K; ++i)
    for (int j = 0; j < in.N; ++j)
    {
      if (!(fin >> in.B.at(i, j)))
        throw std::runtime_error("Malformed matrix B in " + path);
    }

  return in;
}

// Prints a matrix as space-separated integer-looking values (matches the
// spec's example output). Uses %g-style trimming so 58.0 prints as 58.
inline void print_matrix(const Matrix &mat)
{
  std::ostringstream oss;
  for (int i = 0; i < mat.rows; ++i)
  {
    for (int j = 0; j < mat.cols; ++j)
    {
      double v = mat.at(i, j);
      if (v == static_cast<long long>(v))
        oss << static_cast<long long>(v);
      else
        oss << v;
      if (j + 1 < mat.cols)
        oss << ' ';
    }
    oss << '\n';
  }
  std::cout << oss.str();
}

// ---- Algorithms (these are what get timed) ----

// Direct triple-nested-loop GEMM: C = A * B
Matrix gemm_simple(const Matrix &A, const Matrix &B);

// Cache-blocked GEMM: same result as gemm_simple, tiled for cache reuse.
Matrix gemm_blocking(const Matrix &A, const Matrix &B, int block_size);

#endif // GEMM_HPP