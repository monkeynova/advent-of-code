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
  CanReach(const absl::flat_hash_set<Point3>& g, Point3 start, Point3 end)
      : g_(g), cur_(start), end_(end) {}

  Point3 identifier() const override { return cur_; }
  bool IsFinal() const override { return cur_ == end_; }
  void AddNextSteps(State* state) const override {
    for (Point3 dir : Cardinal3::kSixDirs) {
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
  return IntReturn(sides);
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
  Cube cube = {{1, 1, 1}, {1, 1, 1}};
  for (Point3 p : points) cube.ExpandInclude(p);

  std::vector<Point3> test(points.begin(), points.end());
  for (Point3 start : test) {
    for (Point3 start_dir : Cardinal3::kSixDirs) {
      Point3 p = start + start_dir;
      if (points.contains(p)) continue;
      absl::flat_hash_set<Point3> hist = {p};
      bool escaped = false;
      for (std::deque<Point3> queue = {p}; !queue.empty() && !escaped;
           queue.pop_front()) {
        Point3 cur = queue.front();
        for (Point3 dir : Cardinal3::kSixDirs) {
          Point3 test = cur + dir;
          if (!cube.Contains(test)) {
            escaped = true;
            break;
          }
          if (points.contains(test)) continue;
          if (hist.insert(test).second) queue.push_back(test);
        }
      }
      if (!escaped) {
        for (Point3 p2 : hist) points.insert(p2);
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
  return IntReturn(sides);
}

}  // namespace advent_of_code
