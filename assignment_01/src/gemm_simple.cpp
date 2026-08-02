#include "gemm.hpp"

// Direct nested-loop implementation: C[i][j] = sum_k A[i][k] * B[k][j]
// Time complexity: O(M*K*N). No blocking / tiling — straightforward and
// used as the correctness baseline that gemm_blocking must match.
Matrix gemm_simple(const Matrix &A, const Matrix &B)
{
  int M = A.rows, K = A.cols, N = B.cols;
  Matrix C(M, N);

  for (int i = 0; i < M; ++i)
  {
    for (int j = 0; j < N; ++j)
    {
      double sum = 0.0;
      for (int k = 0; k < K; ++k)
      {
        sum += A.at(i, k) * B.at(k, j);
      }
      C.at(i, j) = sum;
    }
  }
  return C;
}