#include "advent_of_code/2015/day02/day02.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_02::Part1(
    absl::Span<std::string_view> input) const {
  int need = 0;
  for (std::string_view dims : input) {
    int l, w, h;
    if (!RE2::FullMatch(dims, "(\\d+)x(\\d+)x(\\d+)", &l, &w, &h)) {
      return absl::InvalidArgumentError(absl::StrCat("Bad input: ", dims));
    }
    need += 2 * l * w + 2 * l * h + 2 * w * h;
    need += std::min(l * w, std::min(l * h, w * h));
  }
  return AdventReturn(need);
}

absl::StatusOr<std::string> Day_2015_02::Part2(
    absl::Span<std::string_view> input) const {
  int need = 0;
  for (std::string_view dims : input) {
    int l, w, h;
    if (!RE2::FullMatch(dims, "(\\d+)x(\\d+)x(\\d+)", &l, &w, &h)) {
      return absl::InvalidArgumentError(absl::StrCat("Bad input: ", dims));
    }
    need += l * w * h;
    need += 2 * (l + w + h) - 2 * std::max(l, std::max(w, h));
  }
  return AdventReturn(need);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/2,
    []() { return std::unique_ptr<AdventDay>(new Day_2015_02()); });

}  // namespace advent_of_code
