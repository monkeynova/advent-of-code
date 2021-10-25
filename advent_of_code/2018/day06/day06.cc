#include "advent_of_code/2018/day06/day06.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day_2018_06::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Point> points;
  PointRectangle r{{0, 0}, {0, 0}};
  for (absl::string_view row : input) {
    Point p;
    if (!RE2::FullMatch(row, "(\\d+), (\\d+)", &p.x, &p.y)) {
      return Error("Bad point: ", row);
    }
    points.push_back(p);
    r.ExpandInclude(p);
  }
  absl::flat_hash_map<Point, int> closest;
  for (Point p : r) {
    int min_dist = std::numeric_limits<int>::max();
    int min_i = -1;
    int dupes = 0;
    for (int i = 0; i < points.size(); ++i) {
      int dist = (points[i] - p).dist();
      if (dist < min_dist) {
        dupes = 1;
        min_i = i;
        min_dist = dist;
      } else if (dist == min_dist) {
        ++dupes;
      }
    }
    if (dupes != 1) min_i = -1;
    closest[p] = min_i;
    VLOG(2) << p << ": " << min_i;
  }
  absl::flat_hash_set<int> permiter;
  for (int x = r.min.x; x <= r.max.x; ++x) {
    permiter.insert(closest[Point{x, r.min.y}]);
    permiter.insert(closest[Point{x, r.max.y}]);
  }
  for (int y = r.min.y; y <= r.max.y; ++y) {
    permiter.insert(closest[Point{r.min.x, y}]);
    permiter.insert(closest[Point{r.max.x, y}]);
  }
  absl::flat_hash_map<int, int> point_counts;
  for (Point p : r) {
    if (!permiter.contains(closest[p])) {
      ++point_counts[closest[p]];
    }
  }
  int max_count = 0;
  for (const auto& [id, count] : point_counts) {
    max_count = std::max(max_count, count);
  }
  return IntReturn(max_count);
}

absl::StatusOr<std::vector<std::string>> Day_2018_06::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<Point> points;
  PointRectangle r{{0, 0}, {0, 0}};
  for (absl::string_view row : input) {
    Point p;
    if (!RE2::FullMatch(row, "(\\d+), (\\d+)", &p.x, &p.y)) {
      return Error("Bad point: ", row);
    }
    points.push_back(p);
    r.ExpandInclude(p);
  }
  int count = 0;
  for (Point p : r) {
    int total_dist = 0;
    for (int i = 0; i < points.size(); ++i) {
      total_dist += (points[i] - p).dist();
    }
    if (total_dist < 10'000) ++count;
  }
  return IntReturn(count);
}

}  // namespace advent_of_code
