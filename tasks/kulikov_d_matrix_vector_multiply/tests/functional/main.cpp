#include <gtest/gtest.h>
#include <vector>
#include <tuple>
#include <stdexcept>

#include "kulikov_d_matrix_vector_multiply/common/include/common.hpp"
#include "kulikov_d_matrix_vector_multiply/mpi/include/ops_mpi.hpp"
#include "kulikov_d_matrix_vector_multiply/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"

namespace kulikov_d_matrix_vector_multiply {

class KulikovMatrixMultiplyRunFuncTests
    : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
public:
    static std::string PrintFuncTestName(const TestType &test_param) {
        return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
    }

protected:
    void SetUp() override {
        // Получаем TestType из FuncTestParam
        const auto& param_tuple = GetParam();                  // FuncTestParam
        const TestType& test_param = std::get<2>(param_tuple); // третий элемент
        int case_id = std::get<0>(test_param);

        switch (case_id) {
            case 0:  // 1x1
                input_data_ = {1, 1, {5}, {3}};
                expected_ = {15};
                break;
            case 1:  // 1x4
                input_data_ = {1, 4, {1,2,3,4}, {1,1,1,1}};
                expected_ = {10};
                break;
            case 2:  // 4x1
                input_data_ = {4, 1, {1,2,3,4}, {2}};
                expected_ = {2,4,6,8};
                break;
            case 3:  // 3x3
                input_data_ = {3,3,{1,2,3,4,5,6,7,8,9},{1,2,3}};
                expected_ = {14,32,50};
                break;
            case 4:  // zeros
                input_data_ = {3,3,std::vector<int>(9,0), std::vector<int>(3,0)};
                expected_ = {0,0,0};
                break;
            default:
                throw std::runtime_error("Unknown test case");
        }
    }

    bool CheckTestOutputData(OutType &output_data) final {
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

// Список тестовых случаев
const std::array<TestType, 5> kTestParams = { {
    std::make_tuple(0, std::string("Single")),
    std::make_tuple(1, std::string("SingleRow")),
    std::make_tuple(2, std::string("SingleCol")),
    std::make_tuple(3, std::string("Square")),
    std::make_tuple(4, std::string("Zeros"))
} };

// Создание задач MPI + SEQ
const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<KulikovDMatrixMultiplyMPI, InType>(kTestParams, PPC_SETTINGS_kulikov_d_matrix_vector_multiply),
    ppc::util::AddFuncTask<KulikovDMatrixMultiplySEQ, InType>(kTestParams, PPC_SETTINGS_kulikov_d_matrix_vector_multiply)
);

// Преобразование в gtest-значения
const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

// Инстанцирование тестов
INSTANTIATE_TEST_SUITE_P(
    MatrixVectorMultiply,
    KulikovMatrixMultiplyRunFuncTests,
    kGtestValues,
    KulikovMatrixMultiplyRunFuncTests::PrintFuncTestName
);

}  // namespace

}  // namespace kulikov_d_matrix_vector_multiply
