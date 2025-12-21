#include "kulikov_d_matrix_vector_multiply/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>
#include <vector>

#include "kulikov_d_matrix_vector_multiply/common/include/common.hpp"
#include "util/include/util.hpp"

namespace kulikov_d_matrix_vector_multiply {

KulikovDMatrixMultiplyMPI::KulikovDMatrixMultiplyMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KulikovDMatrixMultiplySEQ::ValidationImpl() {
  const auto& input = GetInput();

  if (input.rows <= 0 || input.cols <= 0) {
    return false;
  }

  if (input.matrix.size() != static_cast<size_t>(input.rows * input.cols)) {
    return false;
  }

  if (input.vector.size() != static_cast<size_t>(input.cols)) {
    return false;
  }

  return GetOutput().empty();
}

bool KulikovDMatrixMultiplyMPI::PreProcessingImpl() {
  GetOutput() = 2 * GetInput();
  return GetOutput() > 0;
}

bool KulikovDMatrixMultiplyMPI::RunImpl() {
  auto input = GetInput();
  if (input == 0) {
    return false;
  }

  for (InType i = 0; i < GetInput(); i++) {
    for (InType j = 0; j < GetInput(); j++) {
      for (InType k = 0; k < GetInput(); k++) {
        std::vector<InType> tmp(i + j + k, 1);
        GetOutput() += std::accumulate(tmp.begin(), tmp.end(), 0);
        GetOutput() -= i + j + k;
      }
    }
  }

  const int num_threads = ppc::util::GetNumThreads();
  GetOutput() *= num_threads;

  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    GetOutput() /= num_threads;
  } else {
    int counter = 0;
    for (int i = 0; i < num_threads; i++) {
      counter++;
    }

    if (counter != 0) {
      GetOutput() /= counter;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  return GetOutput() > 0;
}

bool KulikovDMatrixMultiplyMPI::PostProcessingImpl() {
  GetOutput() -= GetInput();
  return GetOutput() > 0;
}

}  // namespace kulikov_d_matrix_vector_multiply
