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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_06::Part1(
    absl::Span<absl::string_view> input) const {
  for (int i = 0; i < input[0].size(); ++i) {
    bool found = true;
    absl::flat_hash_set<char> set;
    for (int j = 0; j < 4; ++j) {
      if (set.contains(input[0][i + j])) {
        found = false;
        break;
      }
      set.insert(input[0][i + j]);
    }
    if (found) return IntReturn(i + 4);
  }
  return absl::UnimplementedError("Problem not known");
}

absl::StatusOr<std::string> Day_2022_06::Part2(
    absl::Span<absl::string_view> input) const {
  for (int i = 0; i < input[0].size(); ++i) {
    bool found = true;
    absl::flat_hash_set<char> set;
    for (int j = 0; j < 14; ++j) {
      if (set.contains(input[0][i + j])) {
        found = false;
        break;
      }
      set.insert(input[0][i + j]);
    }
    if (found) return IntReturn(i + 14);
  }
  return absl::UnimplementedError("Problem not known");
}

}  // namespace advent_of_code
