#include "advent_of_code/2021/day20/day20.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/conway.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2021_20::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad input (too small)");
  std::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input (lookup size)");
  if (!input[1].empty()) return Error("Bad input (missing empty line)");

  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input.subspan(2)));

  InfiniteConway conway(b, std::string(lookup));
  RETURN_IF_ERROR(conway.AdvanceN(2));
  return AdventReturn(conway.CountLive());
}

absl::StatusOr<std::string> Day_2021_20::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad input (too small)");
  std::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input (lookup size)");
  if (!input[1].empty()) return Error("Bad input (missing empty line)");

  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input.subspan(2)));

  InfiniteConway conway(b, std::string(lookup));
  RETURN_IF_ERROR(conway.AdvanceN(50));
  return AdventReturn(conway.CountLive());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2021, /*day=*/20, []() {
  return std::unique_ptr<AdventDay>(new Day_2021_20());
});

}  // namespace advent_of_code
