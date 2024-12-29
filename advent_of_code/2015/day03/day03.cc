#include "advent_of_code/2015/day03/day03.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_03::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  absl::flat_hash_map<Point, int> visit;
  Point p{0, 0};
  for (char c : input[0]) {
    ++visit[p];
    p += Cardinal::Parse(c);
  }
  return AdventReturn(visit.size());
}

absl::StatusOr<std::string> Day_2015_03::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  if (input[0].size() % 2 != 0)
    return absl::InvalidArgumentError("Bad input length");
  absl::flat_hash_map<Point, int> visit;
  Point p1{0, 0};
  Point p2{0, 0};
  for (int i = 0; i < input[0].size(); i += 2) {
    ++visit[p1];
    ++visit[p2];
    p1 += Cardinal::Parse(input[0][i]);
    p2 += Cardinal::Parse(input[0][i + 1]);
  }
  return AdventReturn(visit.size());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/3,
    []() { return std::unique_ptr<AdventDay>(new Day_2015_03()); });

}  // namespace advent_of_code
