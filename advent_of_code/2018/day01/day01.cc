// http://adventofcode.com/2018/day/01

#include "advent_of_code/2018/day01/day01.h"

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

absl::StatusOr<std::string> Day_2018_01::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();
  int sum = 0;
  for (int64_t i : *list) sum += i;
  return IntReturn(sum);
}

absl::StatusOr<std::string> Day_2018_01::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();
  absl::flat_hash_set<int> hist;
  int sum = 0;
  while (true) {
    for (int64_t i : *list) {
      if (hist.contains(sum)) return IntReturn(sum);
      hist.insert(sum);
      sum += i;
    }
  }
  return Error("Exited infinite loop");
}

}  // namespace advent_of_code
