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
  const auto& input = GetInput();

  int size = 0;
  int rank = 0;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int rows = input.rows;
  int cols = input.cols;

  int base_rows = rows / size; // мин строк на процесс
  int remainder = rows % size;

  int local_rows = base_rows + (rank < remainder ? 1 : 0);
  int start_row = rank * base_rows + std::min(rank, remainder);

  std::vector<int> local_result(local_rows, 0); // local result

  for (int i = 0; i < local_rows; i++) { // multiply own feed
    int global_row = start_row + i;
    int sum = 0;

    for (int j = 0; j < cols; j++) {
      sum += input.matrix[global_row * cols + j] * input.vector[j];
    }

    local_result[i] = sum;
  }


  if (rank == 0) { // сборка результата на 0 процессе
    for (int i = 0; i < local_rows; i++) {
      GetOutput()[start_row + i] = local_result[i];
    }

    for (int p = 1; p < size; p++) {
      int recv_rows = base_rows + (p < remainder ? 1 : 0);
      int recv_start = p * base_rows + std::min(p, remainder);

      MPI_Recv(GetOutput().data() + recv_start,
               recv_rows,
               MPI_INT,
               p,
               0,
               MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
    }
  } else {
    MPI_Send(local_result.data(),
             local_rows,
             MPI_INT,
             0,
             0,
             MPI_COMM_WORLD);
  }

  return true;
}

bool KulikovDMatrixMultiplyMPI::PostProcessingImpl() {
  return true;
}

}  // namespace kulikov_d_matrix_vector_multiply
