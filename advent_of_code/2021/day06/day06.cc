// https://adventofcode.com/2021/day/6
//
// --- Day 6: Lanternfish ---
// --------------------------
// 
// The sea floor is getting steeper. Maybe the sleigh keys got carried
// this way?
// 
// A massive school of glowing lanternfish swims past. They must spawn
// quickly to reach such large numbers - maybe exponentially quickly? You
// should model their growth rate to be sure.
// 
// Although you know nothing about this specific species of lanternfish,
// you make some guesses about their attributes. Surely, each lanternfish
// creates a new lanternfish once every 7 days.
// 
// However, this process isn't necessarily synchronized between every
// lanternfish - one lanternfish might have 2 days left until it creates
// another lanternfish, while another might have 4. So, you can model
// each fish as a single number that represents the number of days until
// it creates a new lanternfish.
// 
// Furthermore, you reason, a new lanternfish would surely need slightly
// longer before it's capable of producing more lanternfish: two more
// days for its first cycle.
// 
// So, suppose you have a lanternfish with an internal timer value of 3:
// 
// * After one day, its internal timer would become 2.
// 
// * After another day, its internal timer would become 1.
// 
// * After another day, its internal timer would become 0.
// 
// * After another day, its internal timer would reset to 6, and it
// would create a new lanternfish with an internal timer of 8.
// 
// * After another day, the first lanternfish would have an internal
// timer of 5, and the second lanternfish would have an internal
// timer of 7.
// 
// A lanternfish that creates a new fish resets its timer to 6, not 7
// (because 0 is included as a valid timer value). The new lanternfish
// starts with an internal timer of 8 and does not start counting down
// until the next day.
// 
// Realizing what you're trying to do, the submarine automatically
// produces a list of the ages of several hundred nearby lanternfish
// (your puzzle input). For example, suppose you were given the following
// list:
// 
// 3,4,3,1,2
// 
// This list means that the first fish has an internal timer of 3, the
// second fish has an internal timer of 4, and so on until the fifth
// fish, which has an internal timer of 2. Simulating these fish over
// several days would proceed as follows:
// 
// Initial state: 3,4,3,1,2
// After  1 day:  2,3,2,0,1
// After  2 days: 1,2,1,6,0,8
// After  3 days: 0,1,0,5,6,7,8
// After  4 days: 6,0,6,4,5,6,7,8,8
// After  5 days: 5,6,5,3,4,5,6,7,7,8
// After  6 days: 4,5,4,2,3,4,5,6,6,7
// After  7 days: 3,4,3,1,2,3,4,5,5,6
// After  8 days: 2,3,2,0,1,2,3,4,4,5
// After  9 days: 1,2,1,6,0,1,2,3,3,4,8
// After 10 days: 0,1,0,5,6,0,1,2,2,3,7,8
// After 11 days: 6,0,6,4,5,6,0,1,1,2,6,7,8,8,8
// After 12 days: 5,6,5,3,4,5,6,0,0,1,5,6,7,7,7,8,8
// After 13 days: 4,5,4,2,3,4,5,6,6,0,4,5,6,6,6,7,7,8,8
// After 14 days: 3,4,3,1,2,3,4,5,5,6,3,4,5,5,5,6,6,7,7,8
// After 15 days: 2,3,2,0,1,2,3,4,4,5,2,3,4,4,4,5,5,6,6,7
// After 16 days: 1,2,1,6,0,1,2,3,3,4,1,2,3,3,3,4,4,5,5,6,8
// After 17 days: 0,1,0,5,6,0,1,2,2,3,0,1,2,2,2,3,3,4,4,5,7,8
// After 18 days: 6,0,6,4,5,6,0,1,1,2,6,0,1,1,1,2,2,3,3,4,6,7,8,8,8,8
// 
// Each day, a 0 becomes a 6 and adds a new 8 to the end of the list,
// while each other number decreases by 1 if it was present at the start
// of the day.
// 
// In this example, after 18 days, there are a total of 26 fish. After 80
// days, there would be a total of 5934.
// 
// Find a way to simulate lanternfish. How many lanternfish would there
// be after 80 days?
//
// --- Part Two ---
// ----------------
// 
// Suppose the lanternfish live forever and have unlimited food and
// space. Would they take over the entire ocean?
// 
// After 256 days in the example above, there would be a total of
// 26984457539 lanternfish!
// 
// How many lanternfish would there be after 256 days?


#include "advent_of_code/2021/day06/day06.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
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
    absl::Span<absl::string_view> input) const {
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

  return IntReturn(CountPopulationAfter(*nums, kDays));
}

absl::StatusOr<std::string> Day_2021_06::Part2(
    absl::Span<absl::string_view> input) const {
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

  return IntReturn(CountPopulationAfter(*nums, kDays));
}

}  // namespace advent_of_code
