#include "advent_of_code/2021/day13/day13.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/ocr.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::StatusOr<Point> ParseFold(std::string_view fold_str) {
  std::string_view axis_name;
  int64_t val;
  if (!RE2::FullMatch(fold_str, "fold along (x|y)=(\\d+)", &axis_name, &val)) {
    return Error("Bad fold");
  }

  if (axis_name == "x") return Cardinal::kXHat * val;
  if (axis_name == "y") return Cardinal::kYHat * val;
  return Error("Bad axis");
}

absl::StatusOr<absl::flat_hash_set<Point>> Fold(
    absl::flat_hash_set<Point> points, Point axis) {
  if (axis.x != 0 && axis.y != 0) {
    return absl::UnimplementedError("Can't fold on non-axis");
  }
  if (axis.x == 0 && axis.y == 0) {
    return absl::InvalidArgumentError("Bad axis: {0, 0}");
  }
  absl::flat_hash_set<Point> new_points;
  for (Point p : points) {
    if (axis.x != 0) {
      if (p.x > axis.x) {
        if (p.x > 2 * axis.x) return Error("Bad x: ", p.x);
        p.x = 2 * axis.x - p.x;
      }
    } else if (axis.y != 0) {
      if (p.y > axis.y) {
        if (p.y > 2 * axis.y) return Error("Bad y: ", p.y);
        p.y = 2 * axis.y - p.y;
      }
    }
    new_points.insert(p);
  }
  return new_points;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_13::Part1(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_set<Point> points;
  bool folds = false;
  for (std::string_view line : input) {
    if (line.empty()) {
      folds = true;
      continue;
    }
    if (folds) {
      absl::StatusOr<Point> axis = ParseFold(line);
      ASSIGN_OR_RETURN(absl::flat_hash_set<Point> folded,
                       Fold(std::move(points), *axis));
      return AdventReturn(folded.size());
    } else {
      Point p;
      if (!RE2::FullMatch(line, "(\\d+,\\d+)", p.Capture())) {
        return Error("Bad line: ", line);
      }
      points.insert(p);
    }
  }
  return Error("Should not arrive");
}

absl::StatusOr<std::string> Day_2021_13::Part2(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_set<Point> points;
  bool folds = false;
  for (std::string_view line : input) {
    if (line.empty()) {
      folds = true;
      continue;
    }
    if (folds) {
      absl::StatusOr<Point> axis = ParseFold(line);
      ASSIGN_OR_RETURN(points, Fold(std::move(points), *axis));
    } else {
      Point p;
      if (!RE2::FullMatch(line, "(\\d+,\\d+)", p.Capture())) {
        return Error("Bad line: ", line);
      }
      points.insert(p);
    }
  }
  return OCRExtract(CharBoard::DrawNew(points));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2021, /*day=*/13, []() {
  return std::unique_ptr<AdventDay>(new Day_2021_13());
});

}  // namespace advent_of_code
