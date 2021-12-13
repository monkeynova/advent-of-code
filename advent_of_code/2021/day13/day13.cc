#include "advent_of_code/2021/day13/day13.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_13::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Point> points;
  bool folds = false;
  for (absl::string_view p_str: input) {
    if (p_str.empty()) {
      folds = true;
      continue;
    }
    if (folds) {
      absl::string_view axis;
      int64_t val;
      if (!RE2::FullMatch(p_str, "fold along (x|y)=(\\d+)", &axis, &val)) {
        return Error("Bad fold");
      }
      absl::flat_hash_set<Point> fold_set;
      for (Point p : points) {
        if (axis == "x") {
          if (p.x > val) {
            p.x = 2 * val - p.x;
          }
        } else {
          if (axis != "y") return Error("Bad axis");
          if (p.y > val) {
            p.y = 2 * val - p.y;
          }
        }
        fold_set.insert(p);
      }
      return IntReturn(fold_set.size());
    } else {
      Point p;
      if (!RE2::FullMatch(p_str, "(\\d+,\\d+)", p.Capture())) {
        return Error("Bad line: ", p_str);
      }
      points.push_back(p);
    }
  }
  return Error("Should not arrive");
}

absl::StatusOr<std::string> Day_2021_13::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<Point>  points;
  bool folds = false;
  for (absl::string_view p_str: input) {
    if (p_str.empty()) {
      folds = true;
      continue;
    }
    if (folds) {
      absl::string_view axis;
      int64_t val;
      if (!RE2::FullMatch(p_str, "fold along (x|y)=(\\d+)", &axis, &val)) {
        return Error("Bad fold");
      }
      absl::flat_hash_set<Point> new_points;
      for (Point p : points) {
        if (axis == "x") {
          if (p.x > val) {
            p.x = 2 * val - p.x;
          }
        } else {
          if (axis != "y") return Error("Bad axis");
          if (p.y > val) {
            p.y = 2 * val - p.y;
          }
        }        
        new_points.insert(p);
      }
      points = new_points;
    } else {
      Point p;
      if (!RE2::FullMatch(p_str, "(\\d+,\\d+)", p.Capture())) {
        return Error("Bad line: ", p_str);
      }
      points.insert(p);
    }
  }
  PointRectangle r{{0,0}, {0,0}};
  for (Point p : points) { r.ExpandInclude(p); }
  CharBoard b(r);
  for (Point p : points) {
    b[p] = '#';
  }
  return b.AsString();
}

}  // namespace advent_of_code
