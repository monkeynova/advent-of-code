#include "advent_of_code/2019/day09/day09.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"

namespace advent_of_code {
namespace {}  // namespace

absl::StatusOr<std::vector<std::string>> Day09_2019::Part1(
    absl::Span<absl::string_view> input_text) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input_text);
  if (!codes.ok()) return codes.status();

  std::vector<int64_t> input = {1};
  std::vector<int64_t> output;
  if (absl::Status st = codes->Run(input, &output); !st.ok()) return st;

  if (output.size() != 1) {
    return absl::InvalidArgumentError(
        absl::StrCat("Bad output: {", absl::StrJoin(output, ","), "}"));
  }

  return IntReturn(output[0]);
}

absl::StatusOr<std::vector<std::string>> Day09_2019::Part2(
    absl::Span<absl::string_view> input_text) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input_text);
  if (!codes.ok()) return codes.status();

  std::vector<int64_t> input = {2};
  std::vector<int64_t> output;
  if (absl::Status st = codes->Run(input, &output); !st.ok()) return st;

  if (output.size() != 1) {
    return absl::InvalidArgumentError(
        absl::StrCat("Bad output: {", absl::StrJoin(output, ","), "}"));
  }

  return IntReturn(output[0]);
}

}  // namespace advent_of_code
