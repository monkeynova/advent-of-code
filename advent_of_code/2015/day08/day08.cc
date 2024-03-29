#include "advent_of_code/2015/day08/day08.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_08::Part1(
    absl::Span<std::string_view> input) const {
  int delta = 0;
  for (std::string_view str : input) {
    if (str[0] != '"') return Error("Bad input");
    if (str.back() != '"') return Error("Bad input");
    delta += 2;
    for (int i = 1; i < str.size() - 1; ++i) {
      if (str[i] == '\\') {
        if (str[i + 1] == '"' || str[i + 1] == '\\') {
          delta += 1;
          i += 1;
        } else {
          delta += 3;
          i += 3;
        }
      }
    }
  }
  return AdventReturn(delta);
}

absl::StatusOr<std::string> Day_2015_08::Part2(
    absl::Span<std::string_view> input) const {
  int delta = 0;
  for (std::string_view str : input) {
    if (str[0] != '"') return Error("Bad input");
    if (str.back() != '"') return Error("Bad input");
    delta += 4;
    for (int i = 1; i < str.size() - 1; ++i) {
      if (str[i] == '\\') {
        ++delta;
      } else if (str[i] == '"') {
        ++delta;
      }
    }
  }
  return AdventReturn(delta);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/8,
    []() { return std::unique_ptr<AdventDay>(new Day_2015_08()); });

}  // namespace advent_of_code
