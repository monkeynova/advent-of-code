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

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_05::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> jumps = ParseAsInts(input);
  if (!jumps.ok()) return jumps.status();
  int steps = 0;
  for (int ip = 0; ip >= 0 && ip < jumps->size(); ++steps) {
    ip += (*jumps)[ip]++;
  }
  return IntReturn(steps);
}

absl::StatusOr<std::string> Day_2017_05::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> jumps = ParseAsInts(input);
  if (!jumps.ok()) return jumps.status();
  int steps = 0;
  for (int ip = 0; ip >= 0 && ip < jumps->size(); ++steps) {
    int delta = (*jumps)[ip];
    if (delta >= 3) {
      --(*jumps)[ip];
    } else {
      ++(*jumps)[ip];
    }
    ip += delta;
  }
  return IntReturn(steps);
}

}  // namespace advent_of_code
