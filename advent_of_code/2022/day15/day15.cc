// http://adventofcode.com/2022/day/15

#include "advent_of_code/2022/day15/day15.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct SAndB {
  Point sensor;
  Point beacon;
  int d;
};

Interval1D NoCloser(const SAndB& sandb, int y) {
  int dx = sandb.d - abs(sandb.sensor.y - y);
  if (dx < 0) return Interval1D();
  return Interval1D(sandb.sensor.x - dx, sandb.sensor.x + dx + 1);
}

absl::StatusOr<std::vector<SAndB>> ParseList(
    absl::Span<std::string_view> input) {
  static LazyRE2 parse_re = {
      "Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at "
      "x=(-?\\d+), y=(-?\\d+)"};
  std::vector<SAndB> list;
  for (std::string_view line : input) {
    SAndB add;
    if (!RE2::FullMatch(line, *parse_re, &add.sensor.x, &add.sensor.y,
                        &add.beacon.x, &add.beacon.y)) {
      return Error("Bad line: ", line);
    }
    add.d = (add.beacon - add.sensor).dist();
    list.push_back(add);
  }
  return list;
}

bool HasCloser(const std::vector<SAndB>& list, Point t) {
  bool is_closer = false;
  for (const auto& sandb : list) {
    if ((sandb.sensor - t).dist() <= sandb.d) {
      is_closer = true;
    }
  }
  return is_closer;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_15::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<SAndB> list, ParseList(input));

  ASSIGN_OR_RETURN(int y, IntParam());
  Interval1D no_closer;
  Interval1D beacons;
  for (const auto& sandb : list) {
    if (sandb.beacon.y == y) {
      beacons = beacons.Union(Interval1D(sandb.beacon.x, sandb.beacon.x + 1));
    }
    no_closer = no_closer.Union(NoCloser(sandb, y));
  }
  VLOG(1) << "Final: " << no_closer.Minus(beacons);
  return AdventReturn(no_closer.Minus(beacons).Size());
}

absl::StatusOr<std::string> Day_2022_15::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<SAndB> list, ParseList(input));

  ASSIGN_OR_RETURN(int max, IntParam());
  PointRectangle test_area;
  test_area.min = {0, 0};
  test_area.max = Point{max, max};
  std::optional<Point> found;

  // Check for 1 past a boundary by looking for intersections and overlaps in
  // the rotates space {x+y, x-y}. Any point that is in "free space", (and not
  // at the edge) must be one past a boundary, and we check that the point is
  // unique and not next to another in order to prove the answer is unique.
  absl::flat_hash_map<int, int> x_plus_y_to_count;
  absl::flat_hash_map<int, int> x_minus_y_to_count;
  for (const auto& sandb : list) {
    int x_plus_y = sandb.sensor.x + sandb.sensor.y;
    int x_minus_y = sandb.sensor.x - sandb.sensor.y;
    ++x_plus_y_to_count[x_plus_y + sandb.d + 1];
    ++x_plus_y_to_count[x_plus_y - sandb.d - 1];
    ++x_minus_y_to_count[x_minus_y + sandb.d + 1];
    ++x_minus_y_to_count[x_minus_y - sandb.d - 1];
  }
  for (const auto& [x_plus_y, c1] : x_plus_y_to_count) {
    if (c1 < 2) continue;
    for (const auto& [x_minus_y, c2] : x_minus_y_to_count) {
      if (c2 < 2) continue;
      Point t;
      t.x = x_plus_y + x_minus_y;
      if (t.x % 2 != 0) continue;
      t.x /= 2;
      t.y = x_plus_y - x_minus_y;
      if (t.y % 2 != 0) continue;
      t.y /= 2;
      if (test_area.Contains(t) && !HasCloser(list, t)) {
        if (found && *found != t) return Error("Duplicate!");
        found = t;
        VLOG(1) << "Found @" << *found << " -> "
                << found->x * 4000000ll + found->y;
      }
    }
  }
  if (!found) return Error("Not found");
  for (Point d : Cardinal::kFourDirs) {
    if (!HasCloser(list, *found + d)) return Error("Not unique");
  }
  return AdventReturn(found->x * 4000000ll + found->y);
}

}  // namespace advent_of_code
