#include "advent_of_code/2020/day15/day15.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_15::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<std::string_view> pieces = absl::StrSplit(input[0], ",");

  absl::flat_hash_map<int, int> hist;
  int round = 0;
  int last;
  for (; round < pieces.size(); ++round) {
    if (round > 0) {
      hist[last] = round;
    }
    int parsed;
    if (!absl::SimpleAtoi(pieces[round], &parsed))
      return Error("Bad int: ", pieces[round]);
    last = parsed;
    VLOG(1) << last;
  }
  for (; round < 2020; ++round) {
    auto it = hist.find(last);
    int next = it == hist.end() ? 0 : round - it->second;
    if (round > 0) {
      hist[last] = round;
    }
    last = next;
    VLOG(1) << last;
  }
  return AdventReturn(last);
}

absl::StatusOr<std::string> Day_2020_15::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<std::string_view> pieces = absl::StrSplit(input[0], ",");

  absl::flat_hash_map<int, int> hist;
  int round = 0;
  int last;
  for (; round < pieces.size(); ++round) {
    if (round > 0) {
      hist[last] = round;
    }
    int parsed;
    if (!absl::SimpleAtoi(pieces[round], &parsed))
      return Error("Bad int: ", pieces[round]);
    last = parsed;
    VLOG(1) << last;
  }
  for (; round < 30000000; ++round) {
    auto it = hist.find(last);
    int next = it == hist.end() ? 0 : round - it->second;
    if (round > 0) {
      hist[last] = round;
    }
    last = next;
    VLOG(1) << last;
  }
  return AdventReturn(last);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/15, []() {
  return std::unique_ptr<AdventDay>(new Day_2020_15());
});

}  // namespace advent_of_code
