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
#include "re2/re2.h"

ABSL_FLAG(int, advent_day_2022_15_param, -1, "...");

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
    absl::Span<absl::string_view> input) {
  static LazyRE2 parse_re =
      {"Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at "
       "x=(-?\\d+), y=(-?\\d+)"};
  std::vector<SAndB> list;
  for (absl::string_view line : input) {
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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<SAndB>> list = ParseList(input);
  if (!list.ok()) return list.status();

  int y = absl::GetFlag(FLAGS_advent_day_2022_15_param);
  Interval1D no_closer;
  Interval1D beacons;
  for (const auto& sandb : *list) {
    if (sandb.beacon.y == y) {
      beacons = beacons.Union(Interval1D(sandb.beacon.x, sandb.beacon.x + 1));
    }
    no_closer = no_closer.Union(NoCloser(sandb, y));
  }
  VLOG(1) << "Final: " << no_closer.Minus(beacons);
  return IntReturn(no_closer.Minus(beacons).Size());
}

absl::StatusOr<std::string> Day_2022_15::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<SAndB>> list = ParseList(input);
  if (!list.ok()) return list.status();

  int max = absl::GetFlag(FLAGS_advent_day_2022_15_param);
  PointRectangle test_area;
  test_area.min = {0, 0};
  test_area.max = Point{max, max};
  std::optional<Point> found;

  // Check for 1 past a boundary. Can't fully prove unique, but reasonably
  // fast.
  for (const auto& sandb : *list) {
    int d = (sandb.beacon - sandb.sensor).dist();
    for (int i = 0; i <= d + 1; ++i) {
      std::array<Point, 4> to_test = {
        Point{i, (d + 1) - i}, Point{-i, -(d + 1) + i},
        Point{i, (d + 1) - i}, Point{-i, -(d + 1) + i},
      };
      for (Point delta : to_test) {
        Point t = sandb.sensor + delta;
        if (!test_area.Contains(t)) continue;
        if (!HasCloser(*list, t)) {
          if (found && *found != t) return Error("Duplicate!");
          found = t;
          VLOG(1) << "Found @" << *found << " -> "
                  << found->x * 4000000ll + found->y;
        }
      }
    }
  }
  if (!found) return Error("Not found");
  for (Point d : Cardinal::kFourDirs) {
    if (!HasCloser(*list, *found + d)) return Error("Not unique");
  }
  return IntReturn(found->x * 4000000ll + found->y);

  // Check with every 1D interval. Proves unique, but slow.
  for (int y = 0; y <= max; ++y) {
    Interval1D no_closer(0, max);
    Interval1D beacons;
    for (const auto& sandb : *list) {
      no_closer = no_closer.Minus(NoCloser(sandb, y));
      if (no_closer.empty()) break;
    }
    int size = no_closer.Size();
    if (size == 1) {
      if (found) return Error("Multiple points (different y)");
      found = {no_closer.x()[0], y};
      VLOG(1) << "Found @" << *found << " -> "
              << found->x * 4000000ll + found->y;
    } else if (size == 0) {
      if (!no_closer.empty()) return Error("Bad empty");
      continue;
    } else {
      return Error("Multiple points (same y)");
    }
  }
  if (!found) return Error("Not found");
  return IntReturn(found->x * 4000000ll + found->y);
}

}  // namespace advent_of_code
