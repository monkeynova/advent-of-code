#include "advent_of_code/2017/day04/day04.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2017_04::Part1(
    absl::Span<std::string_view> input) const {
  int valid = 0;
  for (std::string_view in : input) {
    std::vector<std::string_view> list = absl::StrSplit(in, " ");
    absl::flat_hash_set<std::string_view> uniq(list.begin(), list.end());
    if (list.size() == uniq.size()) ++valid;
  }
  return AdventReturn(valid);
}

absl::StatusOr<std::string> Day_2017_04::Part2(
    absl::Span<std::string_view> input) const {
  int valid = 0;
  for (std::string_view in : input) {
    std::vector<std::string_view> list = absl::StrSplit(in, " ");
    absl::flat_hash_set<std::string> uniq;
    for (std::string_view w : list) {
      std::string sorted_word = std::string(w);
      std::sort(sorted_word.begin(), sorted_word.end());
      uniq.insert(sorted_word);
    }
    if (list.size() == uniq.size()) ++valid;
  }
  return AdventReturn(valid);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/4,
    []() { return std::unique_ptr<AdventDay>(new Day_2017_04()); });

}  // namespace advent_of_code
