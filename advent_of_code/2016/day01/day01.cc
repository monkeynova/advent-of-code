#include "advent_of_code/2016/day01/day01.h"

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

absl::StatusOr<std::string> Day_2016_01::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::vector<std::string_view> instructions = absl::StrSplit(input[0], ", ");
  Point p = {0, 0};
  Point heading = Cardinal::kNorth;
  for (std::string_view ins : instructions) {
    int dist;
    if (RE2::FullMatch(ins, "L(\\d+)", &dist)) {
      heading = heading.rotate_left();
    } else if (RE2::FullMatch(ins, "R(\\d+)", &dist)) {
      heading = heading.rotate_right();
    } else {
      return Error("Bad instruction: ", ins);
    }
    p += dist * heading;
  }
  return AdventReturn(p.dist());
}

absl::StatusOr<std::string> Day_2016_01::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::vector<std::string_view> instructions = absl::StrSplit(input[0], ", ");
  Point p = {0, 0};
  Point heading = Cardinal::kNorth;
  absl::flat_hash_set<Point> hist = {p};
  for (std::string_view ins : instructions) {
    int dist;
    if (RE2::FullMatch(ins, "L(\\d+)", &dist)) {
      heading = heading.rotate_left();
    } else if (RE2::FullMatch(ins, "R(\\d+)", &dist)) {
      heading = heading.rotate_right();
    } else {
      return Error("Bad instruction: ", ins);
    }
    for (int i = 0; i < dist; ++i) {
      p += heading;
      if (hist.contains(p)) return AdventReturn(p.dist());
      hist.insert(p);
    }
  }
  return Error("No duplicate location");
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/1, []() {
  return std::unique_ptr<AdventDay>(new Day_2016_01());
});

}  // namespace advent_of_code
