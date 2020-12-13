#include "advent_of_code/2015/day12/day12.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day12_2015::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::string_view json = input[0];
  int sum = 0;
  int next = 0;
  while (RE2::FindAndConsume(&json, "(-?\\d+)", &next)) {
    sum += next;
  }
  return IntReturn(sum);
}

absl::StatusOr<std::vector<std::string>> Day12_2015::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}
