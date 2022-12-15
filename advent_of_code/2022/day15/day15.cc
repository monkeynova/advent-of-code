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

namespace advent_of_code {

namespace {

struct SAndB {
  Point sensor;
  Point beacon;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_15::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<SAndB> list;
  for (absl::string_view line : input) {
    SAndB add;
    if (!RE2::FullMatch(line, "Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at x=(-?\\d+), y=(-?\\d+)",
        &add.sensor.x, &add.sensor.y, &add.beacon.x, &add.beacon.y)) {
      return Error("Bad line: ", line);
    }
    list.push_back(add);
  }
  PointRectangle r;
  for (const auto& sandb : list) {
    r.ExpandInclude(sandb.sensor);
    r.ExpandInclude(sandb.beacon);
  }
  int y = r.max.x < 10000 ? 10 : 2000000;
  int count = 0;
  int width = r.max.x - r.min.x + 1;
  for (int x = r.min.x - width; x <= r.max.x + width; ++x) {
    Point t = {x, y};
    bool is_closer = false;
    bool is_beacon = false;
    for (const auto& sandb : list) {
      if (t == sandb.beacon) is_beacon = true;
      if ((sandb.sensor - t).dist() <= (sandb.beacon - sandb.sensor).dist()) {
        is_closer = true;
        ;
      }
    }
    if (is_closer && !is_beacon) {
      ++count;
    }
  }
  return IntReturn(count);
}

bool HasCloser(const std::vector<SAndB>& list, Point t) {
    bool is_closer = false;
    for (const auto& sandb : list) {
      if ((sandb.sensor - t).dist() <= (sandb.beacon - sandb.sensor).dist()) {
        is_closer = true;
      }
    }
    return is_closer;

}

absl::StatusOr<std::string> Day_2022_15::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<SAndB> list;
  for (absl::string_view line : input) {
    SAndB add;
    if (!RE2::FullMatch(line, "Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at x=(-?\\d+), y=(-?\\d+)",
        &add.sensor.x, &add.sensor.y, &add.beacon.x, &add.beacon.y)) {
      return Error("Bad line: ", line);
    }
    list.push_back(add);
  }
  PointRectangle r;
  for (const auto& sandb : list) {
    r.ExpandInclude(sandb.sensor);
    r.ExpandInclude(sandb.beacon);
  }

  Point max = r.max.x < 10000 ? Point{20, 20} : Point{4000000, 4000000};
  std::optional<Point> found;
  for (const auto& sandb : list) {
    int d = (sandb.beacon - sandb.sensor).dist();
    for (int i = 0; i <= d + 1; ++i) {
      std::vector<Point> to_test;
      to_test.push_back({sandb.sensor.x + i, sandb.sensor.y + (d + 1) - i});
      to_test.push_back({sandb.sensor.x - i, sandb.sensor.y + (d + 1) - i});
      to_test.push_back({sandb.sensor.x + i, sandb.sensor.y - (d + 1) + i});
      to_test.push_back({sandb.sensor.x - i, sandb.sensor.y - (d + 1) + i});
      for (Point t : to_test) {
        if (t.x < 0) continue;
        if (t.y < 0) continue;
        if (t.x > max.x) continue;
        if (t.y > max.y) continue;
        if (!HasCloser(list, t)) {
          if (found && *found != t) return Error("Duplicate!");
          found = t;
          VLOG(1) << "Found @" << *found << " -> " << found->x * 4000000ll + found->y;
        }
      }
    }
  }
  if (!found) return Error("Not found");
  return IntReturn(found->x * 4000000ll + found->y);
#if 0
  PointRectangle test;
  test.min = {0, 0};
  test.max = r.max.x < 10000 ? Point{20, 20} : Point{4000000, 4000000};
  std::optional<Point> found;
  for (Point t : test) {
    bool is_closer = false;
    for (const auto& sandb : list) {
      if ((sandb.sensor - t).dist() <= (sandb.beacon - sandb.sensor).dist()) {
        is_closer = true;
      }
    }
    if (is_closer) continue;
    if (found) return Error("Duplicate!");
    found = t;
    VLOG(1) << "Found @" << *found << " -> " << found->x * 4000000 + found->y;
  }
  if (!found) return Error("Not found");
  return IntReturn(found->x * 4000000 + found->y);
#endif
}

}  // namespace advent_of_code
