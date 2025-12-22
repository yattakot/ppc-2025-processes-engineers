#include <gtest/gtest.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "kulikov_d_matrix_vector_multiply/common/include/common.hpp"
#include "kulikov_d_matrix_vector_multiply/mpi/include/ops_mpi.hpp"
#include "kulikov_d_matrix_vector_multiply/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace kulikov_d_matrix_vector_multiply {

class KulikovMatrixMultiplyRunFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    const auto& [type, _] =
        std::get<static_cast<size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());

    input_data_ = GenerateInput(type);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return (input_data_ == output_data);
  }

  InType GetTestInputData() final {
    return input_data_;
  }
private:
  static InType GenerateInput(MatrixType type) {
    MatrixType input{};

    switch (type) {
      case MatrixType::kSingleConstant:
        input.rows = 1;
        input.cols = 1;
        input.matrix = {5};
        input.vector = {3};
        break;

      case MatrixType::kSingleRow:
        input.rows = 1;
        input.cols = 4;
        input.matrix = {1, 2, 3, 4};
        input.vector = {1, 1, 1, 1};
        break;

      case MatrixType::kSingleCol:
        input.rows = 4;
        input.cols = 1;
        input.matrix = {1, 2, 3, 4};
        input.vector = {2};
        break;

      case MatrixType::kSquare:
        input.rows = 3;
        input.cols = 3;
        input.matrix = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        input.vector = {1, 2, 3};
        break;

      case MatrixType::kAllZeros:
        input.rows = 3;
        input.cols = 3;
        input.matrix.assign(9, 0);
        input.vector.assign(3, 0);
        break;

      case MatrixType::kMixedSigns:
        input.rows = 2;
        input.cols = 3;
        input.matrix = {
            -1,  2, -3,
             4, -5,  6
        };
        input.vector = {1, -1, 2};
        break;

      default:
        throw std::runtime_error("Unsupported MatrixType");
    }

    return input;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(KulikovMatrixMultiplyRunFuncTests, MatmulFromPic) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 3> kTestParam = {std::make_tuple(3, "3"), std::make_tuple(5, "5"), std::make_tuple(7, "7")};

const auto kTestTasksList =
    std::tuple_cat(ppc::util::AddFuncTask<KulikovDMatrixMultiplyMPI, InType>(kTestParam, PPC_SETTINGS_kulikov_d_matrix_vector_multiply),
                   ppc::util::AddFuncTask<KulikovDMatrixMultiplySEQ, InType>(kTestParam, PPC_SETTINGS_kulikov_d_matrix_vector_multiply));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = KulikovMatrixMultiplyRunFuncTests::PrintFuncTestName<KulikovMatrixMultiplyRunFuncTests>;

INSTANTIATE_TEST_SUITE_P(PicMatrixTests, KulikovMatrixMultiplyRunFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace kulikov_d_matrix_vector_multiply
