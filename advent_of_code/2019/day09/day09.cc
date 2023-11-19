#include "advent_of_code/2019/day09/day09.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2019_09::Part1(
    absl::Span<std::string_view> input_text) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input_text);
  if (!codes.ok()) return codes.status();

  std::vector<int64_t> input = {1};
  std::vector<int64_t> output;
  RETURN_IF_ERROR(codes->Run(input, &output));

  if (output.size() != 1) {
    return absl::InvalidArgumentError(
        absl::StrCat("Bad output: {", absl::StrJoin(output, ","), "}"));
  }

  return AdventReturn(output[0]);
}

absl::StatusOr<std::string> Day_2019_09::Part2(
    absl::Span<std::string_view> input_text) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input_text);
  if (!codes.ok()) return codes.status();

  std::vector<int64_t> input = {2};
  std::vector<int64_t> output;
  RETURN_IF_ERROR(codes->Run(input, &output));

  if (output.size() != 1) {
    return absl::InvalidArgumentError(
        absl::StrCat("Bad output: {", absl::StrJoin(output, ","), "}"));
  }

  return AdventReturn(output[0]);
}

}  // namespace advent_of_code
