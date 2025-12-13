#pragma once

#include "kulikov_d_coun_number_char/common/include/common.hpp"
#include "task/include/task.hpp"

namespace kulikov_d_coun_number_char {

class KulikovDiffCountNumberCharMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit KulikovDiffCountNumberCharMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace kulikov_d_coun_number_char
