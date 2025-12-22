#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "kulikov_d_matrix_vector_multiply/common/include/common.hpp"
#include "kulikov_d_matrix_vector_multiply/mpi/include/ops_mpi.hpp"
#include "kulikov_d_matrix_vector_multiply/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"

namespace kulikov_d_matrix_vector_multiply {

class KulikovMatrixMultiplyRunFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    const auto& [case_id, _] =
        std::get<static_cast<size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());

    switch (case_id) {
      case 0:  // 1x1
        input_data_ = {1, 1, {5}, {3}};
        expected_ = {15};
        break;

      case 1:  // 1x4
        input_data_ = {1, 4, {1, 2, 3, 4}, {1, 1, 1, 1}};
        expected_ = {10};
        break;

      case 2:  // 4x1
        input_data_ = {4, 1, {1, 2, 3, 4}, {2}};
        expected_ = {2, 4, 6, 8};
        break;

      case 3:  // 3x3
        input_data_ = {
            3, 3,
            {1, 2, 3,
             4, 5, 6,
             7, 8, 9},
            {1, 2, 3}
        };
        expected_ = {14, 32, 50};
        break;

      case 4:  // zeros
        input_data_ = {3, 3, std::vector<int>(9, 0), std::vector<int>(3, 0)};
        expected_ = {0, 0, 0};
        break;

      default:
        throw std::runtime_error("Unknown test case");
    }
  }


  bool CheckTestOutputData(OutType& output_data) final {
    return output_data == expected_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType expected_;
};

namespace {

TEST_P(KulikovMatrixMultiplyRunFuncTests, CorrectMatrixVectorMultiply) {
  ExecuteTest(GetParam());
}

const std::vector<TestType> kTestParams = {
    {0, "Single"},
    {1, "SingleRow"},
    {2, "SingleCol"},
    {3, "Square"},
    {4, "Zeros"},
};

const auto kTasks =
    std::tuple_cat(
        ppc::util::AddFuncTask<KulikovDMatrixMultiplyMPI, InType>(
            kTestParams, PPC_SETTINGS_kulikov_d_matrix_vector_multiply),
        ppc::util::AddFuncTask<KulikovDMatrixMultiplySEQ, InType>(
            kTestParams, PPC_SETTINGS_kulikov_d_matrix_vector_multiply));

const auto kValues = ppc::util::ExpandToValues(kTasks);

INSTANTIATE_TEST_SUITE_P(
    MatrixVectorMultiply,
    KulikovMatrixMultiplyRunFuncTests,
    kValues,
    KulikovMatrixMultiplyRunFuncTests::PrintFuncTestName<
        KulikovMatrixMultiplyRunFuncTests>);

}  // namespace

}  // namespace kulikov_d_matrix_vector_multiply
