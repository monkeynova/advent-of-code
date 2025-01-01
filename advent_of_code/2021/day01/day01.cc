#include "advent_of_code/2021/day01/day01.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2021_01::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> list, ParseAsInts(input));

  int64_t count = 0;
  for (int i = 0; i + 1 < list.size(); ++i) {
    if (list[i] < list[i + 1]) ++count;
  }

  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2021_01::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> list, ParseAsInts(input));

  int64_t count = 0;
  for (int i = 0; i + 3 < list.size(); ++i) {
    if (list[i] < list[i + 3]) ++count;
  }

  return AdventReturn(count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2021, /*day=*/1,
    []() { return std::unique_ptr<AdventDay>(new Day_2021_01()); });

}  // namespace advent_of_code
