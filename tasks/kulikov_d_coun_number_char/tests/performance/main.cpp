#include <gtest/gtest.h>

#include "kulikov_d_coun_number_char/common/include/common.hpp"
#include "kulikov_d_coun_number_char/mpi/include/ops_mpi.hpp"
#include "kulikov_d_coun_number_char/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace kulikov_d_coun_number_char {

class KulikovDCounNumberCharPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 100;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(KulikovDCounNumberCharPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, NesterovATestTaskMPI, NesterovATestTaskSEQ>(PPC_SETTINGS_kulikov_d_coun_number_char);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = KulikovDCounNumberCharPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KulikovDCounNumberCharPerfTests, kGtestValues, kPerfTestName);

}  // namespace kulikov_d_coun_number_char
