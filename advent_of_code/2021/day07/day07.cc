#include "advent_of_code/2021/day07/day07.h"

#include "absl/algorithm/container.h"
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

absl::StatusOr<std::string> Day_2021_07::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::StatusOr<std::vector<int64_t>> nums =
      ParseAsInts(absl::StrSplit(input[0], ","));
  if (!nums.ok()) return nums.status();

  // Median minimizes cost function. And if median is between two values any
  // point between the two values is valid, so we don't need to worry about
  // odd vs even size for `nums`.
  absl::c_sort(*nums);
  int64_t dest = nums->at((nums->size() + 1) / 2);
  int64_t cost = 0;
  for (int64_t n : *nums) {
    cost += abs(n - dest);
  }
  return IntReturn(cost);
}

absl::StatusOr<std::string> Day_2021_07::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::StatusOr<std::vector<int64_t>> nums =
      ParseAsInts(absl::StrSplit(input[0], ","));
  if (!nums.ok()) return nums.status();

  const auto& [min_it, max_it] = absl::c_minmax_element(*nums);
  int64_t best_cost = std::numeric_limits<int64_t>::max();
  for (int i = *min_it; i <= *max_it; ++i) {
    int64_t cost = 0;
    for (int64_t n : *nums) {
      int64_t d = abs(n - i);
      cost += d * (d + 1) / 2;
    }
    if (cost < best_cost) {
      best_cost = cost;
    }
    // Cost curve is concave up. Once we bounce, stop.
    if (cost > best_cost) break;
  }

  return IntReturn(best_cost);
}

}  // namespace advent_of_code
