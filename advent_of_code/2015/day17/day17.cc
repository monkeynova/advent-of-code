// https://adventofcode.com/2015/day/17
//
// --- Day 17: No Such Thing as Too Much ---
// -----------------------------------------
//
// The elves bought too much eggnog again - 150 liters this time. To fit
// it all into your refrigerator, you'll need to move it into smaller
// containers. You take an inventory of the capacities of the available
// containers.
//
// For example, suppose you have containers of size 20, 15, 10, 5, and 5
// liters. If you need to store 25 liters, there are four ways to do it:
//
// * 15 and 10
//
// * 20 and 5 (the first 5)
//
// * 20 and 5 (the second 5)
//
// * 15, 5, and 5
//
// Filling all containers entirely, how many different combinations of
// containers can exactly fit all 150 liters of eggnog?
//
// --- Part Two ---
// ----------------
//
// While playing with all the containers in the kitchen, another load of
// eggnog arrives! The shipping and receiving department is requesting as
// many containers as you can spare.
//
// Find the minimum number of containers that can exactly fit all 150
// liters of eggnog. How many different ways can you fill that number of
// containers and still hold exactly 150 litres?
//
// In the example above, the minimum number of containers was two. There
// were three ways to use that many containers, and so the answer there
// would be 3.

#include "advent_of_code/2015/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_17::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> cap = ParseAsInts(input);
  if (!cap.ok()) return cap.status();

  int valid = 0;
  for (int i = 0; i < (1 << cap->size()); ++i) {
    int sum = 0;
    for (int bit = 0; (1 << bit) <= i; ++bit) {
      if (i & (1 << bit)) {
        sum += (*cap)[bit];
      }
    }
    if (sum == 150) ++valid;
  }

  return IntReturn(valid);
}

absl::StatusOr<std::string> Day_2015_17::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> cap = ParseAsInts(input);
  if (!cap.ok()) return cap.status();

  int min_valid = std::numeric_limits<int>::max();
  int valid = 0;
  for (int i = 0; i < (1 << cap->size()); ++i) {
    int sum = 0;
    int entries = 0;
    for (int bit = 0; (1 << bit) <= i; ++bit) {
      if (i & (1 << bit)) {
        sum += (*cap)[bit];
        ++entries;
      }
    }
    if (sum == 150) {
      if (entries == min_valid) {
        ++valid;
      } else if (entries < min_valid) {
        min_valid = entries;
        valid = 1;
      }
    }
  }

  return IntReturn(valid);
}

}  // namespace advent_of_code
