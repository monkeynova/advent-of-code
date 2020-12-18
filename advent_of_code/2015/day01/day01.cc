#include "advent_of_code/2015/day01/day01.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {}  // namespace

absl::StatusOr<std::vector<std::string>> Day01_2015::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  int floor = 0;
  for (char c : input[0]) {
    if (c == '(')
      ++floor;
    else if (c == ')')
      --floor;
    else
      return absl::InvalidArgumentError("Bad char");
  }
  return IntReturn(floor);
}

absl::StatusOr<std::vector<std::string>> Day01_2015::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  int floor = 0;
  for (int i = 0; i < input[0].size(); ++i) {
    char c = input[0][i];
    if (c == '(')
      ++floor;
    else if (c == ')')
      --floor;
    else
      return absl::InvalidArgumentError("Bad char");
    if (floor < 0) return IntReturn(i + 1);
  }
  return absl::InvalidArgumentError("No basement");
}

}  // namespace advent_of_code
