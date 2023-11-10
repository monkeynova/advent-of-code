#include "advent_of_code/2017/day02/day02.h"

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

absl::StatusOr<std::string> Day_2017_02::Part1(
    absl::Span<std::string_view> input) const {
  int checksum = 0;
  for (std::string_view row : input) {
    std::vector<std::string_view> nums = absl::StrSplit(row, "\t");
    int max = std::numeric_limits<int>::min();
    int min = std::numeric_limits<int>::max();
    for (std::string_view num_str : nums) {
      int num;
      if (!absl::SimpleAtoi(num_str, &num)) return Error("Bad num: ", num_str);
      max = std::max(max, num);
      min = std::min(min, num);
    }
    checksum += max - min;
  }
  return AdventReturn(checksum);
}

absl::StatusOr<std::string> Day_2017_02::Part2(
    absl::Span<std::string_view> input) const {
  int checksum = 0;
  for (std::string_view row : input) {
    std::vector<std::string_view> num_strs = absl::StrSplit(row, "\t");
    absl::StatusOr<std::vector<int64_t>> nums = ParseAsInts(num_strs);
    if (!nums.ok()) return nums.status();
    bool found = false;
    for (int64_t a : *nums) {
      for (int64_t b : *nums) {
        if (a <= b) continue;
        if (a % b == 0) {
          if (found) return Error("Dual divisible: ", row);
          found = true;
          checksum += a / b;
        }
      }
    }
    if (!found) return Error("No divisors found: ", row);
  }
  return AdventReturn(checksum);
}

}  // namespace advent_of_code
