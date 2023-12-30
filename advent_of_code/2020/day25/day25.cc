#include "advent_of_code/2020/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/mod.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_25::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  ASSIGN_OR_RETURN(std::vector<int64_t> ints, ParseAsInts(input));

  for (int i = 1; i <= 20201227; ++i) {
    if (PowerMod<int64_t>(7, i, 20201227) == ints[0]) {
      return AdventReturn(PowerMod<int64_t>(ints[1], i, 20201227));
    }
    if (PowerMod<int64_t>(7, i, 20201227) == ints[1]) {
      return AdventReturn(PowerMod<int64_t>(ints[0], i, 20201227));
    }
  }

  return Error("No value found");
}

absl::StatusOr<std::string> Day_2020_25::Part2(
    absl::Span<std::string_view> input) const {
  return "Merry Christmas!";
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/25,
    []() { return std::unique_ptr<AdventDay>(new Day_2020_25()); });

}  // namespace advent_of_code
