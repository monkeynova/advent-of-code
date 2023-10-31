// http://adventofcode.com/2022/day/18

#include "advent_of_code/2022/day18/day18.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/point3.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2022_18::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<Point3> points;
  for (absl::string_view line : input) {
    Point3 p;
    if (!RE2::FullMatch(line, "([\\d,]+)", p.Capture()))
      return Error("Bad line");
    points.insert(p);
  }
  int sides = 0;
  for (Point3 p : points) {
    for (Point3 dir : Cardinal3::kSixDirs) {
      if (points.contains(p + dir)) continue;
      ++sides;
    }
  }
  return AdventReturn(sides);
};

absl::StatusOr<std::string> Day_2022_18::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<Point3> points;
  for (absl::string_view line : input) {
    Point3 p;
    if (!RE2::FullMatch(line, "([\\d,]+)", p.Capture()))
      return Error("Bad line");
    points.insert(p);
  }
  if (points.size() < 1) return Error("No points");
  Cube cube = {*points.begin(), *points.begin()};
  for (Point3 p : points) cube.ExpandInclude(p);

  std::vector<Point3> test(points.begin(), points.end());
  for (Point3 start : test) {
    for (Point3 start_dir : Cardinal3::kSixDirs) {
      Point3 p = start + start_dir;
      if (points.contains(p)) continue;

      bool escaped = false;
      auto hist = Point3Walk({
        .start = p,
        .is_good = [&](Point3 test, int) { return !points.contains(test); },
        .is_final = [&](Point3 test) {
          if (cube.Contains(test)) return false;
          escaped = true;
          return true;
        }
      }).FindReachable();
      if (!escaped) {
        for (const auto& [p2, _] : hist) points.insert(p2);
      }
    }
  }

  int sides = 0;
  for (Point3 p : points) {
    for (Point3 dir : Cardinal3::kSixDirs) {
      if (points.contains(p + dir)) continue;
      ++sides;
    }
  }
  return AdventReturn(sides);
}

}  // namespace advent_of_code
