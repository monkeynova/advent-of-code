#include "advent_of_code/2019/day05/day05.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2019_05::Part1(
    absl::Span<std::string_view> input_text) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input_text);
  if (!codes.ok()) return codes.status();

  std::vector<int64_t> input = {1};
  std::vector<int64_t> output;
  RETURN_IF_ERROR(codes->Run(input, &output));

  for (int i = 0; i < output.size() - 1; ++i) {
    if (output[i] != 0) {
      return absl::InvalidArgumentError("Bad diagnostic!");
    }
  }

  return AdventReturn(output.back());
}

absl::StatusOr<std::string> Day_2019_05::Part2(
    absl::Span<std::string_view> input_text) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input_text);
  if (!codes.ok()) return codes.status();

  std::vector<int64_t> input = {5};
  std::vector<int64_t> output;
  RETURN_IF_ERROR(codes->Run(input, &output));

  for (int i = 0; i < output.size() - 1; ++i) {
    if (output[i] != 0) {
      return absl::InvalidArgumentError("Bad diagnostic!");
    }
  }

  return AdventReturn(output.back());
}

}  // namespace advent_of_code
