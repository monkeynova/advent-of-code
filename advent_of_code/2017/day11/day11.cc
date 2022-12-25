#include "advent_of_code/2017/day11/day11.h"

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

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_11::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Point p = Cardinal::kOrigin;
  std::vector<absl::string_view> dirs = absl::StrSplit(input[0], ",");
  for (absl::string_view dir : dirs) {
    if (dir == "nw")
      p += Cardinal::kNorthWest;
    else if (dir == "ne")
      p += Cardinal::kNorthEast;
    else if (dir == "sw")
      p += Cardinal::kSouthWest;
    else if (dir == "se")
      p += Cardinal::kSouthEast;
    else if (dir == "n")
      p += 2 * Cardinal::kNorth;
    else if (dir == "s")
      p += 2 * Cardinal::kSouth;
    else
      return Error("Bad direction: ", dir);
  }
  return AdventReturn(p.dist() / 2);
}

absl::StatusOr<std::string> Day_2017_11::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Point p = Cardinal::kOrigin;
  int max_dist = std::numeric_limits<int>::min();
  std::vector<absl::string_view> dirs = absl::StrSplit(input[0], ",");
  for (absl::string_view dir : dirs) {
    if (dir == "nw")
      p += Cardinal::kNorthWest;
    else if (dir == "ne")
      p += Cardinal::kNorthEast;
    else if (dir == "sw")
      p += Cardinal::kSouthWest;
    else if (dir == "se")
      p += Cardinal::kSouthEast;
    else if (dir == "n")
      p += 2 * Cardinal::kNorth;
    else if (dir == "s")
      p += 2 * Cardinal::kSouth;
    else
      return Error("Bad direction: ", dir);
    max_dist = std::max(max_dist, p.dist() / 2);
  }
  return AdventReturn(max_dist);
}

}  // namespace advent_of_code
