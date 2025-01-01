#include "advent_of_code/2018/day01/day01.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2018_01::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> list, ParseAsInts(input));
  return AdventReturn(absl::c_accumulate(list, 0));
}

absl::StatusOr<std::string> Day_2018_01::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> list, ParseAsInts(input));
  absl::flat_hash_set<int> hist;
  int sum = 0;
  while (true) {
    for (int64_t i : list) {
      if (hist.contains(sum)) return AdventReturn(sum);
      hist.insert(sum);
      sum += i;
    }
  }
  return Error("Exited infinite loop");
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2018, /*day=*/1,
    []() { return std::unique_ptr<AdventDay>(new Day_2018_01()); });

}  // namespace advent_of_code
