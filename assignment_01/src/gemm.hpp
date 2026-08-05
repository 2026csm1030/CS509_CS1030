#ifndef GEMM_HPP
#define GEMM_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

using namespace std;

// Simple row-major matrix wrapper over a flat vector.

struct Matrix
{
  int rows = 0;
  int cols = 0;

  vector<double> data;

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

inline GemmInput read_gemm_input(const string &path)
{
  ifstream fin(path);

  if (!fin.is_open())
  {
    throw runtime_error("Could not open input file: " + path);
  }

  GemmInput in;
  if (!(fin >> in.M >> in.K >> in.N))
  {
    throw runtime_error("Malformed header in " + path);
  }

  if (in.M <= 0 || in.K <= 0 || in.N <= 0)
  {
    throw runtime_error("Invalid dimensions in " + path);
  }

  in.A = Matrix(in.M, in.K);

  for (int i = 0; i < in.M; ++i)
  {
    for (int j = 0; j < in.K; ++j)
    {
      if (!(fin >> in.A.at(i, j)))
        throw runtime_error("Malformed matrix A in " + path);
    }
  }

  in.B = Matrix(in.K, in.N);

  for (int i = 0; i < in.K; ++i)
  {
    for (int j = 0; j < in.N; ++j)
    {
      if (!(fin >> in.B.at(i, j)))
        throw runtime_error("Malformed matrix B in " + path);
    }
  }

  return in;
}

inline void print_matrix(const Matrix &mat)
{
  ostringstream oss;

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

  cout << oss.str();
}

Matrix gemm_simple(const Matrix &A, const Matrix &B);

Matrix gemm_blocking(const Matrix &A, const Matrix &B, int block_size);

#endif