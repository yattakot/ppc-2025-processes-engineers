#include <gtest/gtest.h>
#include <vector>
#include <numeric>
#include <cstddef>

#include "kulikov_d_matrix_vector_multiply/common/include/common.hpp"
#include "kulikov_d_matrix_vector_multiply/mpi/include/ops_mpi.hpp"
#include "kulikov_d_matrix_vector_multiply/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace kulikov_d_matrix_vector_multiply {

struct KulikovMatrixInput {
    std::vector<int> flat_matrix;
    std::vector<int> vector;
    int rows;
    int cols;
};

class KulikovMatrixPerf : public ppc::util::BaseRunPerfTests<KulikovMatrixInput, OutType> {
protected:
    static constexpr int kRows = 10000;
    static constexpr int kCols = 10000;

    KulikovMatrixInput input_;
    OutType expected_;

    void SetUp() override {
        // Создаём матрицу как 2D, затем сплющиваем
        std::vector<std::vector<int>> matrix2d(kRows, std::vector<int>(kCols));
        std::vector<int> vec(kCols);

        for (int r = 0; r < kRows; ++r) {
            for (int c = 0; c < kCols; ++c) {
                // Чередуем значения: чётные строки положительные, нечётные отрицательные
                int val = (r + c) * ((r % 2 == 0) ? 1 : -1);
                matrix2d[r][c] = val;
            }
        }

        for (int c = 0; c < kCols; ++c) {
            vec[c] = ((c % 3) == 0) ? c : -c;
        }

        // Сплющиваем матрицу
        input_.flat_matrix.resize(kRows * kCols);
        for (int r = 0; r < kRows; ++r) {
            std::copy(matrix2d[r].begin(), matrix2d[r].end(),
                      input_.flat_matrix.begin() + r * kCols);
        }

        input_.vector = vec;
        input_.rows = kRows;
        input_.cols = kCols;

        expected_.resize(kRows, 0);
    }

    // Проверка результата: размер + базовая сумма
    bool CheckTestOutputData(OutType &output) final {
        if (output.size() != static_cast<size_t>(kRows)) return false;
        long long sum = std::accumulate(output.begin(), output.end(), 0LL);
        return sum != 0;  // просто чтобы проверить, что массив не пустой и «обработан»
    }

    KulikovMatrixInput GetTestInputData() final {
        return input_;
    }
};

TEST_P(KulikovMatrixPerf, RunPerfModes) {
    ExecuteTest(GetParam());
}

const auto all_perf_tasks = ppc::util::MakeAllPerfTasks<
    KulikovMatrixInput,
    KulikovDMatrixMultiplyMPI,
    KulikovDMatrixMultiplySEQ
>(PPC_SETTINGS_kulikov_d_matrix_vector_multiply);

const auto gtest_values = ppc::util::TupleToGTestValues(all_perf_tasks);

const auto perf_test_name = KulikovMatrixPerf::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, KulikovMatrixPerf, gtest_values, perf_test_name);

}  // namespace kulikov_d_matrix_vector_multiply
