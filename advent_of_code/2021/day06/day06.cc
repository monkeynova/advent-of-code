#include "advent_of_code/2021/day06/day06.h"

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

int64_t SimulateBrute(std::vector<int64_t> fish, int64_t steps) {
  for (int day = 0; day < steps; ++day) {
    VLOG(1) << day << ": " << fish.size();
    int64_t to_add = 0;
    for (int64_t& n : fish) {
      --n;
      if (n < 0) {
        n = 6;
        ++to_add;
      }
    }
    for (int i = 0; i < to_add; ++i) {
      fish.push_back(8);
    }
  }
    
  return fish.size();
}

int64_t PerFish(int64_t start, int64_t steps) {
  if (steps == 0) return 1;

  static absl::flat_hash_map<std::pair<int64_t, int64_t>, int64_t> memo;
  auto key = std::make_pair(start, steps);
  if (auto it = memo.find(key); it != memo.end()) return it->second;

  int64_t ret = 0;
  if (start == 0) {
    ret += PerFish(6, steps - 1) + PerFish(8, steps - 1);
  } else {
    ret += PerFish(start - 1, steps - 1);
  }

  return memo[key] = ret;
}

int64_t SimulateMemo(std::vector<int64_t> fish, int64_t steps) {
  int64_t sum = 0;
  for (int64_t start : fish) {
    sum += PerFish(start, steps);
  }
  return sum;
}


}  // namespace

absl::StatusOr<std::string> Day_2021_06::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<int64_t> nums;
  for (absl::string_view num_str : absl::StrSplit(input[0], ",")) {
    int64_t num;
    if (!absl::SimpleAtoi(num_str, &num)) return Error("Bad num: ", num_str);
    nums.push_back(num);
  }

  return IntReturn(SimulateBrute(nums, 80));

}

absl::StatusOr<std::string> Day_2021_06::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<int64_t> nums;
  for (absl::string_view num_str : absl::StrSplit(input[0], ",")) {
    int64_t num;
    if (!absl::SimpleAtoi(num_str, &num)) return Error("Bad num: ", num_str);
    nums.push_back(num);
  }

  return IntReturn(SimulateMemo(nums, 256));
}

}  // namespace advent_of_code
