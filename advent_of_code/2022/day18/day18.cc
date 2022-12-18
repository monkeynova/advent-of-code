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
#include "advent_of_code/bfs.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class CanReach : public BFSInterface<CanReach, Point3> {
 public:
  CanReach(const absl::flat_hash_set<Point3>& g, Point3 start, Point3 end) : g_(g), cur_(start), end_(end) {}

  Point3 identifier() const override { return cur_; }
  bool IsFinal() const override { return cur_ == end_; }
  void AddNextSteps(State* state) const override {
    for (Point3 dir : Cardinal3::kNeighborDirs) {
      if (dir.dist() != 1) continue;
      if (g_.contains(cur_ + dir)) continue;
      CanReach next = *this;
      next.cur_ += dir;
      state->AddNextStep(next);
    }
  }

 private:
  const absl::flat_hash_set<Point3>& g_;
  Point3 cur_;
  Point3 end_;
};

}  // namespace

absl::StatusOr<std::string> Day_2022_18::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<Point3> points;
  for (absl::string_view line : input) {
    Point3 p;
    if (!RE2::FullMatch(line, "([\\d,]+)", p.Capture())) return Error("Bad line");
    points.insert(p);
  }
  int sides = 0;
  for (Point3 p : points) {
    for (Point3 dir : Cardinal3::kNeighborDirs) {
      if (dir.dist() != 1) continue;
      if (points.contains(p + dir)) continue;
      ++sides;
    }
  }
  return IntReturn(sides);
};

absl::StatusOr<std::string> Day_2022_18::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<Point3> points;
  for (absl::string_view line : input) {
    Point3 p;
    if (!RE2::FullMatch(line, "([\\d,]+)", p.Capture())) return Error("Bad line");
    points.insert(p);
  }
  Cube cube = {*points.begin(), *points.begin()};
  for (Point3 p : points) {
    cube.min.x = std::min(cube.min.x, p.x);
    cube.min.y = std::min(cube.min.y, p.y);
    cube.min.z = std::min(cube.min.z, p.z);
    cube.max.x = std::max(cube.max.x, p.x);
    cube.max.y = std::max(cube.max.y, p.y);
    cube.max.z = std::max(cube.max.z, p.z);
  }

  Point3 out = cube.min - Cardinal3::kXHat;

  VLOG(1) << "Dist to " << out;

  for (Point3 p : cube) {
    absl::optional<int> dist = CanReach(points, p, out).FindMinSteps();
    if (!dist) points.insert(p);
  }

  int sides = 0;
  for (Point3 p : points) {
    for (Point3 dir : Cardinal3::kNeighborDirs) {
      if (dir.dist() != 1) continue;
      if (points.contains(p + dir)) continue;
      ++sides;
    }
  }
  return IntReturn(sides);
}

}  // namespace advent_of_code
