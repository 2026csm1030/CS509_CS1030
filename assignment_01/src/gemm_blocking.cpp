#include "gemm.hpp"
#include <algorithm>

// Blocked/tiled GEMM. Splits the i, j, k loops into chunks of `block_size`
// so that the sub-blocks of A, B, and C being worked on at any moment are
// small enough to stay resident in cache, improving data reuse compared to
// gemm_simple. Produces the same numeric result as gemm_simple.
Matrix gemm_blocking(const Matrix &A, const Matrix &B, int block_size)
{
  int M = A.rows, K = A.cols, N = B.cols;
  Matrix C(M, N);

  if (block_size <= 0)
    block_size = M; // fallback: behaves like simple

  for (int ii = 0; ii < M; ii += block_size)
  {
    int i_max = std::min(ii + block_size, M);
    for (int jj = 0; jj < N; jj += block_size)
    {
      int j_max = std::min(jj + block_size, N);
      for (int kk = 0; kk < K; kk += block_size)
      {
        int k_max = std::min(kk + block_size, K);

        // Multiply the current (block_size x block_size) tiles.
        for (int i = ii; i < i_max; ++i)
        {
          for (int j = jj; j < j_max; ++j)
          {
            double sum = C.at(i, j);
            for (int k = kk; k < k_max; ++k)
            {
              sum += A.at(i, k) * B.at(k, j);
            }
            C.at(i, j) = sum;
          }
        }
      }
    }
  }
  return C;
}