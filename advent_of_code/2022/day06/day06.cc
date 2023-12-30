#include "advent_of_code/2022/day06/day06.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::StatusOr<int> FindMarker(std::string_view line, int len) {
  for (int i = 0; i < line.size() - len; ++i) {
    std::string_view sub = line.substr(i, len);
    absl::flat_hash_set<char> set(sub.begin(), sub.end());
    if (sub.size() == set.size()) return i + len;
  }
  return Error("No marker found");
}

}  // namespace

absl::StatusOr<std::string> Day_2022_06::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  return AdventReturn(FindMarker(input[0], 4));
}

absl::StatusOr<std::string> Day_2022_06::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  return AdventReturn(FindMarker(input[0], 14));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/6,
    []() { return std::unique_ptr<AdventDay>(new Day_2022_06()); });

}  // namespace advent_of_code
