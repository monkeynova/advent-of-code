#include "advent_of_code/2019/day5/day5.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"

absl::StatusOr<std::vector<std::string>> Day5_2019::Part1(
    const std::vector<absl::string_view>& input_text) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input_text);
  if (!codes.ok()) return codes.status();

  std::vector<int64_t> input = {1};
  std::vector<int64_t> output;
  if (absl::Status st = codes->Run(input, &output); !st.ok()) return st;

  for (int i = 0; i < output.size() - 1; ++i) {
    if (output[i] != 0) {
      return absl::InvalidArgumentError("Bad diagnostic!");
    }
  }

  return IntReturn(output.back());
}

absl::StatusOr<std::vector<std::string>> Day5_2019::Part2(
    const std::vector<absl::string_view>& input_text) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input_text);
  if (!codes.ok()) return codes.status();

  std::vector<int64_t> input = {5};
  std::vector<int64_t> output;
  if (absl::Status st = codes->Run(input, &output); !st.ok()) return st;

  for (int i = 0; i < output.size() - 1; ++i) {
    if (output[i] != 0) {
      return absl::InvalidArgumentError("Bad diagnostic!");
    }
  }

  return IntReturn(output.back());
}
