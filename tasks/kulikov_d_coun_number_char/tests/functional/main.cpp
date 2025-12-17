#include <gtest/gtest.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <tuple>
#include <array>

#include "kulikov_d_coun_number_char/common/include/common.hpp"
#include "kulikov_d_coun_number_char/mpi/include/ops_mpi.hpp"
#include "kulikov_d_coun_number_char/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace kulikov_d_coun_number_char {

class KulikovDiffCountNumberCharFuncTests
    : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const testing::TestParamInfo<std::tuple<
      std::function<std::shared_ptr<ppc::task::Task<InType, OutType>>(InType)>,
      std::string,
      TestType>>& param)
  {
    const TestType& test_param = std::get<2>(param.param);
    return FormatName(test_param.first) + "_" + std::to_string(test_param.second);
  }

 protected:
  void SetUp() override {
    TestType test_param = std::get<2>(GetParam());
    expected_output_ = test_param.second;

    const std::string file_path = ppc::util::GetAbsoluteTaskPath(PPC_ID_kulikov_d_coun_number_char, test_param.first);
    std::ifstream file(file_path);
    if (!file.is_open()) {
      throw std::runtime_error("Cannot open file: " + file_path);
    }

    std::string line1, line2;
    if (!std::getline(file, line1)) throw std::runtime_error("Failed reading first line");
    if (!std::getline(file, line2)) throw std::runtime_error("Failed reading second line");

    TrimEnd(line1);
    TrimEnd(line2);

    std::string extra;
    if (std::getline(file, extra) && !extra.empty())
      throw std::runtime_error("Unexpected extra data in file: " + file_path);

    input_data_ = std::make_pair(line1, line2);
    file.close();
  }

  bool CheckTestOutputData(OutType& output_data) final {
    return output_data == expected_output_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_ = {"", ""};
  OutType expected_output_ = 0;

  static void TrimEnd(std::string& s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) s.pop_back();
  }

  static std::string FormatName(const std::string& s) {
    size_t dot_pos = s.find_last_of('.');
    return (dot_pos != std::string::npos) ? s.substr(0, dot_pos) : s;
  }
};

namespace {

const std::array<TestType, 5> kTestParam = {
    std::make_pair("test_empty_first.txt", 3),
    std::make_pair("test_empty_second.txt", 3),
    std::make_pair("test_identical.txt", 0),
    std::make_pair("test_single_diff.txt", 1),
    std::make_pair("test_diff_length.txt", 2)
};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<KulikovDiffCountNumberCharMPI, InType>(kTestParam,
        PPC_SETTINGS_kulikov_d_coun_number_char),
    ppc::util::AddFuncTask<KulikovDiffCountNumberCharSEQ, InType>(kTestParam,
        PPC_SETTINGS_kulikov_d_coun_number_char)
);

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kFuncTestName = KulikovDiffCountNumberCharFuncTests::PrintTestParam;

INSTANTIATE_TEST_SUITE_P(KulikovFuncTests, KulikovDiffCountNumberCharFuncTests,kGtestValues, kFuncTestName);

}  // namespace

}  // namespace kulikov_d_coun_number_char
