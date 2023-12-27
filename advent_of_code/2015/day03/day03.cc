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
    switch (c) {
      case '^':
        p += Cardinal::kNorth;
        break;
      case 'v':
        p += Cardinal::kSouth;
        break;
      case '<':
        p += Cardinal::kWest;
        break;
      case '>':
        p += Cardinal::kEast;
        break;
      default:
        return absl::InvalidArgumentError("Bad direction");
    }
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
    switch (input[0][i]) {
      case '^':
        p1 += Cardinal::kNorth;
        break;
      case 'v':
        p1 += Cardinal::kSouth;
        break;
      case '<':
        p1 += Cardinal::kWest;
        break;
      case '>':
        p1 += Cardinal::kEast;
        break;
      default:
        return absl::InvalidArgumentError("Bad direction");
    }
    switch (input[0][i + 1]) {
      case '^':
        p2 += Cardinal::kNorth;
        break;
      case 'v':
        p2 += Cardinal::kSouth;
        break;
      case '<':
        p2 += Cardinal::kWest;
        break;
      case '>':
        p2 += Cardinal::kEast;
        break;
      default:
        return absl::InvalidArgumentError("Bad direction");
    }
  }
  return AdventReturn(visit.size());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/3, []() {
  return std::unique_ptr<AdventDay>(new Day_2015_03());
});

}  // namespace advent_of_code
