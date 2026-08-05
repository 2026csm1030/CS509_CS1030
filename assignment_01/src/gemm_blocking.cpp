#include "gemm.hpp"
#include <algorithm>

using namespace std;

// Blocked/tiled GEMM. Splits the i, j, k loops into chunks of `block_size`
// so that the sub-blocks of A, B, and C being worked on at any moment are
// small enough to stay resident in cache, improving data reuse compared to
// gemm_simple. Produces the same numeric result as gemm_simple.

Matrix gemm_blocking(const Matrix &A, const Matrix &B, int block_size)
{
  int M = A.rows, K = A.cols, N = B.cols;
  Matrix C(M, N);

  if (block_size <= 0)
  {
    // fallback: behaves like simple
    block_size = M;
  }

  for (int ii = 0; ii < M; ii += block_size)
  {
    int i_max = min(ii + block_size, M);

    for (int jj = 0; jj < N; jj += block_size)
    {
      int j_max = min(jj + block_size, N);

      for (int kk = 0; kk < K; kk += block_size)
      {
        int k_max = min(kk + block_size, K);

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