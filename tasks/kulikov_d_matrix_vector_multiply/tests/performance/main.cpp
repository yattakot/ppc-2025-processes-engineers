#include <gtest/gtest.h>
#include <vector>
#include <random>

#include "kulikov_d_matrix_vector_multiply/common/include/common.hpp"
#include "kulikov_d_matrix_vector_multiply/mpi/include/ops_mpi.hpp"
#include "kulikov_d_matrix_vector_multiply/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace kulikov_d_matrix_vector_multiply {

class KulikovMatrixMultiplyRunPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
protected:
    InType input_data_;

    void SetUp() override {
        const int rows = 1000;
        const int cols = 1000;
        input_data_.rows = rows;
        input_data_.cols = cols;

        std::mt19937 gen(12345);
        std::uniform_int_distribution<int> dist(-100, 100);

        input_data_.matrix.resize(static_cast<size_t>(rows * cols));
        for (auto &v : input_data_.matrix) {
            v = dist(gen);
        }

        input_data_.vector.resize(static_cast<size_t>(cols));
        for (auto &v : input_data_.vector) {
            v = dist(gen);
        }
    }

    bool CheckTestOutputData(OutType &output_data) final {
        return output_data.size() == static_cast<size_t>(input_data_.rows);
    }

    InType GetTestInputData() final {
        return input_data_;
    }
};

TEST_P(KulikovMatrixMultiplyRunPerfTests, RunPerfModes) {
    ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KulikovDMatrixMultiplyMPI, KulikovDMatrixMultiplySEQ>(
        PPC_SETTINGS_kulikov_d_matrix_vector_multiply);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);
const auto kPerfTestName = KulikovMatrixMultiplyRunPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KulikovMatrixMultiplyRunPerfTests, kGtestValues, kPerfTestName);

} // namespace kulikov_d_matrix_vector_multiply
