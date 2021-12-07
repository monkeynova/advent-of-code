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
  std::vector<int64_t> nums;
  for (absl::string_view num_str : absl::StrSplit(input[0], ",")) {
    int64_t num;
    if (!absl::SimpleAtoi(num_str, &num)) return Error("Bad num: ", num_str);
    nums.push_back(num);
  }

  int64_t min = nums[0];
  int64_t max = nums[0];
  for (int64_t n : nums) {
    min = std::min(n, min);
    max = std::max(n, max);
  }
  int64_t best_cost = std::numeric_limits<int64_t>::max();
  for (int i = min; i <= max; ++i) {
    int64_t cost = 0;
    for (int64_t n : nums) {
      cost += abs(n - i);
    }
    if (cost < best_cost) {
      best_cost = cost;
    }
  }
  
  return IntReturn(best_cost);
}

absl::StatusOr<std::string> Day_2021_07::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<int64_t> nums;
  for (absl::string_view num_str : absl::StrSplit(input[0], ",")) {
    int64_t num;
    if (!absl::SimpleAtoi(num_str, &num)) return Error("Bad num: ", num_str);
    nums.push_back(num);
  }

  int64_t min = nums[0];
  int64_t max = nums[0];
  for (int64_t n : nums) {
    min = std::min(n, min);
    max = std::max(n, max);
  }
  int64_t best_cost = std::numeric_limits<int64_t>::max();
  for (int i = min; i <= max; ++i) {
    int64_t cost = 0;
    for (int64_t n : nums) {
      int64_t d = abs(n - i);
      cost += d * (d + 1) / 2;
    }
    if (cost < best_cost) {
      best_cost = cost;
    }
  }
  
  return IntReturn(best_cost);
}

}  // namespace advent_of_code
