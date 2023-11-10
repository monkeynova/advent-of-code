#include "advent_of_code/2021/day06/day06.h"

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

int64_t CountPopulationAfterBrute(std::vector<int64_t> fish, int64_t steps) {
  for (int day = 0; day < steps; ++day) {
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

int64_t CountPopulationAfterUniquePop(std::vector<int64_t> fish,
                                      int64_t steps) {
  absl::flat_hash_map<int64_t, int64_t> population;
  for (int64_t f : fish) ++population[f];
  for (int day = 0; day < steps; ++day) {
    absl::flat_hash_map<int64_t, int64_t> new_pop;
    for (const auto& [n, count] : population) {
      if (n == 0) {
        new_pop[6] += count;
        new_pop[8] += count;
      } else {
        new_pop[n - 1] += count;
      }
    }
    population = std::move(new_pop);
  }

  int64_t total_count = 0;
  for (const auto& [_, count] : population) {
    total_count += count;
  }
  return total_count;
}

int64_t CountFishDescendentsAfter(int64_t start, int64_t steps) {
  // Fish doesn't get a chance to breed.
  if (steps <= start) return 1;

  static absl::flat_hash_map<std::pair<int64_t, int64_t>, int64_t> memo;
  auto key = std::make_pair(start, steps);
  if (auto it = memo.find(key); it != memo.end()) return it->second;

  // Count descendents after next breeding step.
  return memo[key] = CountFishDescendentsAfter(6, steps - start - 1) +
                     CountFishDescendentsAfter(8, steps - start - 1);
}

int64_t CountPopulationAfterMemo(std::vector<int64_t> fish, int64_t steps) {
  int64_t sum = 0;
  for (int64_t start : fish) {
    sum += CountFishDescendentsAfter(start, steps);
  }
  return sum;
}

int64_t CountPopulationAfter(std::vector<int64_t> fish, int64_t steps) {
  return CountPopulationAfterMemo(fish, steps);
}

}  // namespace

absl::StatusOr<std::string> Day_2021_06::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::StatusOr<std::vector<int64_t>> nums =
      ParseAsInts(absl::StrSplit(input[0], ","));
  if (!nums.ok()) return nums.status();

  constexpr int64_t kDays = 80;

  if (run_audit()) {
    if (CountPopulationAfter(*nums, kDays) !=
        CountPopulationAfterBrute(*nums, kDays)) {
      return Error("Bad Brute");
    }
    if (CountPopulationAfter(*nums, kDays) !=
        CountPopulationAfterUniquePop(*nums, kDays)) {
      return Error("Bad UniquePop");
    }
  }

  return AdventReturn(CountPopulationAfter(*nums, kDays));
}

absl::StatusOr<std::string> Day_2021_06::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::StatusOr<std::vector<int64_t>> nums =
      ParseAsInts(absl::StrSplit(input[0], ","));
  if (!nums.ok()) return nums.status();

  constexpr int64_t kDays = 256;

  if (run_audit()) {
    if (CountPopulationAfter(*nums, kDays) !=
        CountPopulationAfterUniquePop(*nums, kDays)) {
      return Error("Bad UniquePop");
    }
  }

  return AdventReturn(CountPopulationAfter(*nums, kDays));
}

}  // namespace advent_of_code
