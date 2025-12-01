#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace kulikov_d_coun_number_char {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace kulikov_d_coun_number_char
