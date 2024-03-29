#include "advent_of_code/2022/day01/day01.h"

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

absl::StatusOr<std::string> Day_2022_01::Part1(
    absl::Span<std::string_view> input) const {
  int best = 0;
  int cur = 0;
  for (std::string_view line : input) {
    int next;
    if (line.empty()) {
      if (cur > best) {
        best = cur;
      }
      cur = 0;
    } else if (absl::SimpleAtoi(line, &next)) {
      cur += next;
    } else {
      return Error("Bad line: ", line);
    }
  }
  if (cur > best) {
    best = cur;
  }
  return AdventReturn(best);
}

absl::StatusOr<std::string> Day_2022_01::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<int> elves;
  int cur = 0;
  for (std::string_view line : input) {
    int next;
    if (line.empty()) {
      elves.push_back(cur);
      cur = 0;
    } else if (absl::SimpleAtoi(line, &next)) {
      cur += next;
    } else {
      return Error("Bad line: ", line);
    }
  }
  elves.push_back(cur);
  if (elves.size() < 3) {
    return Error("Too few elves (need 3, have ", elves.size(), ")");
  }
  absl::c_sort(elves);
  absl::c_reverse(elves);
  return AdventReturn(elves[0] + elves[1] + elves[2]);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/1,
    []() { return std::unique_ptr<AdventDay>(new Day_2022_01()); });

}  // namespace advent_of_code
