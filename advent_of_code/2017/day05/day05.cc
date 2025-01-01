#include "advent_of_code/2017/day05/day05.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2017_05::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> jumps, ParseAsInts(input));
  int steps = 0;
  for (int ip = 0; ip >= 0 && ip < jumps.size(); ++steps) {
    ip += jumps[ip]++;
  }
  return AdventReturn(steps);
}

absl::StatusOr<std::string> Day_2017_05::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> jumps, ParseAsInts(input));
  int steps = 0;
  for (int ip = 0; ip >= 0 && ip < jumps.size(); ++steps) {
    int delta = jumps[ip];
    if (delta >= 3) {
      --jumps[ip];
    } else {
      ++jumps[ip];
    }
    ip += delta;
  }
  return AdventReturn(steps);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/5,
    []() { return std::unique_ptr<AdventDay>(new Day_2017_05()); });

}  // namespace advent_of_code
