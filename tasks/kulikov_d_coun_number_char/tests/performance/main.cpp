#include <gtest/gtest.h>

#include <random>
#include <string>
#include <tuple>
#include <utility>

#include "kulikov_d_coun_number_char/common/include/common.hpp"
#include "kulikov_d_coun_number_char/mpi/include/ops_mpi.hpp"
#include "kulikov_d_coun_number_char/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace kulikov_d_coun_number_char {

class KulikovDiffCountNumberCharPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  InType input_data_ = {"", ""};
  OutType expected_output_ = 0;

  size_t base_length_ = 100000;  // длина базовой строки
  int scale_factor_ = 10;        // масштабирование для увеличения размера
  unsigned int seed_ = 42;       // генератор случайных чисел
  float diff_rate_ = 0.05F;      // доля символов, которые отличаются

  void SetUp() override {
    auto [s1, s2, diff_count] = GenerateTestData(base_length_, seed_, diff_rate_, scale_factor_);
    input_data_ = std::make_pair(s1, s2);
    expected_output_ = diff_count;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == expected_output_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  static std::tuple<std::string, std::string, int> GenerateTestData(size_t base_length, unsigned int seed,
                                                                    float diff_rate = 0.1F, int scale_factor = 1) {
    std::mt19937 gen(seed);
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::uniform_int_distribution<size_t> char_dist(0, charset.size() - 1);
    std::uniform_real_distribution<double> diff_dist(0.0, 1.0);

    std::string base_str1, base_str2;
    int base_diff_count = 0;
    base_str1.reserve(base_length);
    base_str2.reserve(base_length);

    // Генерация базового паттерна
    for (size_t i = 0; i < base_length; ++i) {
      char c1 = charset[char_dist(gen)];
      base_str1.push_back(c1);

      if (diff_dist(gen) < diff_rate) {
        char c2 = charset[char_dist(gen)];
        while (c2 == c1) {
          c2 = charset[char_dist(gen)];
        }
        base_str2.push_back(c2);
        base_diff_count++;
      } else {
        base_str2.push_back(c1);
      }
    }

    // Масштабирование строк
    std::string s1, s2;
    size_t scaled_length = base_length * scale_factor;
    s1.reserve(scaled_length);
    s2.reserve(scaled_length);
    for (int i = 0; i < scale_factor; ++i) {
      s1 += base_str1;
      s2 += base_str2;
    }

    int total_diff_count = base_diff_count * scale_factor;

    // Дополнение первой строки для проверки разной длины
    size_t extension = scaled_length / 1000;  // 0.1% дополнительной длины
    for (size_t i = 0; i < extension; ++i) {
      s1.push_back(charset[char_dist(gen)]);
    }
    total_diff_count += static_cast<int>(extension);

    return {s1, s2, total_diff_count};
  }
};

TEST_P(KulikovDiffCountNumberCharPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

// Генерация всех perf задач для MPI и SEQ
const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, KulikovDiffCountNumberCharMPI, KulikovDiffCountNumberCharSEQ>(
        PPC_SETTINGS_kulikov_d_coun_number_char);

// Преобразование в формат gtest
const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

// Имя теста
const auto kPerfTestName = KulikovDiffCountNumberCharPerfTests::CustomPerfTestName;

// Инстанцирование perf-тестов
INSTANTIATE_TEST_SUITE_P(RunModeTests, KulikovDiffCountNumberCharPerfTests, kGtestValues, kPerfTestName);

}  // namespace kulikov_d_coun_number_char
