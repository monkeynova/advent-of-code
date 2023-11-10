#include "advent_of_code/2015/day01/day01.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_01::Part1(
    absl::Span<std::string_view> input) const {
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
  return AdventReturn(floor);
}

absl::StatusOr<std::string> Day_2015_01::Part2(
    absl::Span<std::string_view> input) const {
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
    if (floor < 0) return AdventReturn(i + 1);
  }
  return absl::InvalidArgumentError("No basement");
}

}  // namespace advent_of_code
