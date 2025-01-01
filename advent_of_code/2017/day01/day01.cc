#include "advent_of_code/2017/day01/day01.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2017_01::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int sum = 0;
  int size = input[0].size();
  for (int i = 0; i < size; ++i) {
    if (input[0][i] == input[0][(i + 1) % size]) {
      sum += input[0][i] - '0';
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2017_01::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int sum = 0;
  int size = input[0].size();
  for (int i = 0; i < size; ++i) {
    if (input[0][i] == input[0][(i + (size / 2)) % size]) {
      sum += input[0][i] - '0';
    }
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/1,
    []() { return std::unique_ptr<AdventDay>(new Day_2017_01()); });

}  // namespace advent_of_code
