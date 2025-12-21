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
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    GetOutput().assign(GetInput().rows, 0);
  }

  return true;
}

bool KulikovDMatrixMultiplyMPI::RunImpl() {
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int rows = input.rows;

  int base_rows = rows / size; // мин строк на процесс
  int remainder = rows % size;

  int local_rows = base_rows + (rank < remainder ? 1 : 0);
  int start_row = rank * base_rows + std::min(rank, remainder);
}

bool KulikovDMatrixMultiplyMPI::PostProcessingImpl() {
  GetOutput() -= GetInput();
  return GetOutput() > 0;
}

}  // namespace kulikov_d_matrix_vector_multiply
