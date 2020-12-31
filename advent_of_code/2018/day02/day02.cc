#include "advent_of_code/2018/day02/day02.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day02_2018::Part1(
    absl::Span<absl::string_view> input) const {
  int twos = 0;
  int threes = 0;
  for (absl::string_view row : input) {
    absl::flat_hash_map<char, int> counts;
    for (char c : row) ++counts[c];
    int has_two = false;
    int has_three = false;
    for (const auto& [_, count] : counts) {
      if (count == 2) has_two = true;
      if (count == 3) has_three = true;
    }
    if (has_two) ++twos;
    if (has_three) ++threes;
  }
  return IntReturn(twos * threes);
}

absl::StatusOr<std::vector<std::string>> Day02_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
