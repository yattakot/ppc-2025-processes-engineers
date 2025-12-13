#include "kulikov_d_coun_number_char/mpi/include/ops_mpi.hpp"

#include <mpi.h>
#include <algorithm>

#include <numeric>
#include <vector>
#include <cstddef>
#include <functional>

#include "kulikov_d_coun_number_char/common/include/common.hpp"
#include "util/include/util.hpp"

namespace kulikov_d_coun_number_char {

KulikovDiffCountNumberCharMPI::KulikovDiffCountNumberCharMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool KulikovDiffCountNumberCharMPI::ValidationImpl() {
  return (GetOutput() == 0);
}

bool KulikovDiffCountNumberCharMPI::PreProcessingImpl() {
  return true;
}

bool KulikovDiffCountNumberCharMPI::RunImpl() {
    size_t shorter_len = 0;
    size_t longer_len = 0;

    if (proc_rank_ == 0) {
        const auto &[first_str, second_str] = GetInput();
        shorter_len = std::min(first_str.size(), second_str.size());
        longer_len = std::max(first_str.size(), second_str.size());
    }

    MPI_Bcast(&shorter_len, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&longer_len, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    std::vector<char> chunk1;
    std::vector<char> chunk2;

    // раздаем участки строк всем процессам
    DistributeData(shorter_len, chunk1, chunk2);

    int local_mismatches = 0;
    for (size_t i = 0; i < chunk1.size(); ++i) {
        if (chunk1[i] != chunk2[i]) {
            local_mismatches++;
        }
    }

    int total_mismatches = 0;
    MPI_Allreduce(&local_mismatches, &total_mismatches, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    GetOutput() = total_mismatches + static_cast<int>(longer_len - shorter_len);

    return true;
}

bool KulikovDiffCountNumberCharMPI::PostProcessingImpl() {
  return true;
}

}  // namespace kulikov_d_coun_number_char
