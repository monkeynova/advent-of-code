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
#include "re2/re2.h"

ABSL_FLAG(int, advent_day_2022_15_param, -1, "...");

namespace advent_of_code {

namespace {

struct SAndB {
  Point sensor;
  Point beacon;
  int d;
};

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

  PointRectangle r;
  absl::flat_hash_set<Point> beacons;
  for (const auto& sandb : *list) {
    r.ExpandInclude(sandb.sensor);
    r.ExpandInclude(sandb.beacon);
    beacons.insert(sandb.beacon);
  }
  int y = absl::GetFlag(FLAGS_advent_day_2022_15_param);
  int count = 0;
  int width = r.max.x - r.min.x + 1;
  for (int x = r.min.x - width; x <= r.max.x + width; ++x) {
    Point t = {x, y};
    if (HasCloser(*list, t) && !beacons.contains(t)) {
      ++count;
    }
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2022_15::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<SAndB>> list = ParseList(input);
  if (!list.ok()) return list.status();

  PointRectangle test_area;
  test_area.min = {0, 0};
  int max = absl::GetFlag(FLAGS_advent_day_2022_15_param);
  test_area.max = Point{max, max};
  std::optional<Point> found;
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
  return IntReturn(found->x * 4000000ll + found->y);
}

}  // namespace advent_of_code
