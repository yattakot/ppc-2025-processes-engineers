#include "kulikov_d_coun_number_char/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <vector>

#include "kulikov_d_coun_number_char/common/include/common.hpp"
#include "util/include/util.hpp"

namespace kulikov_d_coun_number_char {

KulikovDiffCountNumberCharMPI::KulikovDiffCountNumberCharMPI(const InType &in) {
  MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank_);
  MPI_Comm_size(MPI_COMM_WORLD, &proc_size_);

  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KulikovDiffCountNumberCharMPI::ValidationImpl() {
  return true;
}

bool KulikovDiffCountNumberCharMPI::PreProcessingImpl() {
  return true;
}

bool KulikovDiffCountNumberCharMPI::RunImpl() {
  std::string s1, s2;

  if (proc_rank_ == 0) {
    const auto &input = GetInput();
    s1 = input.first;
    s2 = input.second;
  }

  size_t len1 = s1.size();
  size_t len2 = s2.size();
  MPI_Bcast(&len1, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  MPI_Bcast(&len2, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

  s1.resize(len1);
  s2.resize(len2);

  MPI_Bcast(s1.data(), len1, MPI_CHAR, 0, MPI_COMM_WORLD);
  MPI_Bcast(s2.data(), len2, MPI_CHAR, 0, MPI_COMM_WORLD);

  const size_t min_len = std::min(len1, len2);
  const size_t max_len = std::max(len1, len2);

  const size_t base = min_len / proc_size_;
  const size_t rem = min_len % proc_size_;
  const size_t begin = proc_rank_ * base + std::min(static_cast<size_t>(proc_rank_), rem);
  const size_t end = begin + base + (static_cast<size_t>(proc_rank_) < rem ? 1 : 0);

  int local_diff = 0;
  for (size_t i = begin; i < end; ++i) {
    if (s1[i] != s2[i]) {
      local_diff++;
    }
  }

  int global_diff = 0;
  MPI_Allreduce(&local_diff, &global_diff, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  GetOutput() = global_diff + static_cast<int>(max_len - min_len);
  return true;
}

bool KulikovDiffCountNumberCharMPI::PostProcessingImpl() {
  return true;
}

}  // namespace kulikov_d_coun_number_char
