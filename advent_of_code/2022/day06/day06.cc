// http://adventofcode.com/2022/day/06

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

absl::StatusOr<int> FindMarker(absl::string_view line, int len) {
  for (int i = 0; i < line.size() - len; ++i) {
    bool found = true;
    absl::flat_hash_set<char> set;
    for (int j = 0; j < len; ++j) {
      if (set.contains(line[i + j])) {
        found = false;
        break;
      }
      set.insert(line[i + j]);
    }
    if (found) return i + len;
  }
  return Error("No marker found");
}

}  // namespace

absl::StatusOr<std::string> Day_2022_06::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  return IntReturn(FindMarker(input[0], 4));
}

absl::StatusOr<std::string> Day_2022_06::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  return IntReturn(FindMarker(input[0], 14));
}

}  // namespace advent_of_code
