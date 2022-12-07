// https://adventofcode.com/2021/day/13
//
// --- Day 13: Transparent Origami ---
// -----------------------------------
// 
// You reach another volcanically active part of the cave. It would be
// nice if you could do some kind of thermal imaging so you could tell
// ahead of time which caves are too hot to safely enter.
// 
// Fortunately, the submarine seems to be equipped with a thermal camera!
// When you activate it, you are greeted with:
// 
// Congratulations on your purchase! To activate this infrared thermal imaging
// camera system, please enter the code found on page 1 of the manual.
// 
// Apparently, the Elves have never used this feature. To your surprise,
// you manage to find the manual; as you go to open it, page 1 falls out.
// It's a large sheet of transparent paper! The transparent paper is
// marked with random dots and includes instructions on how to fold it up
// (your puzzle input). For example:
// 
// 6,10
// 0,14
// 9,10
// 0,3
// 10,4
// 4,11
// 6,0
// 6,12
// 4,1
// 0,13
// 10,12
// 3,4
// 3,0
// 8,4
// 1,10
// 2,14
// 8,10
// 9,0
// 
// fold along y=7
// fold along x=5
// 
// The first section is a list of dots on the transparent paper. 0,0
// represents the top-left coordinate. The first value, x, increases to
// the right. The second value, y, increases downward. So, the coordinate
// 3,0 is to the right of 0,0, and the coordinate 0,7 is below 0,0. The
// coordinates in this example form the following pattern, where # is a
// dot on the paper and . is an empty, unmarked position:
// 
// ...#..#..#.
// ....#......
// ...........
// #..........
// ...#....#.#
// ...........
// ...........
// ...........
// ...........
// ...........
// .#....#.##.
// ....#......
// ......#...#
// #..........
// #.#........
// 
// Then, there is a list of fold instructions. Each instruction indicates
// a line on the transparent paper and wants you to fold the paper up
// (for horizontal y=... lines) or left (for vertical x=... lines). In
// this example, the first fold instruction is fold along y=7, which
// designates the line formed by all of the positions where y is 7
// (marked here with -):
// 
// ...#..#..#.
// ....#......
// ...........
// #..........
// ...#....#.#
// ...........
// ...........
// -----------
// ...........
// ...........
// .#....#.##.
// ....#......
// ......#...#
// #..........
// #.#........
// 
// Because this is a horizontal line, fold the bottom half up. Some of
// the dots might end up overlapping after the fold is complete, but dots
// will never appear exactly on a fold line. The result of doing this
// fold looks like this:
// 
// #.##..#..#.
// #...#......
// ......#...#
// #...#......
// .#.#..#.###
// ...........
// ...........
// 
// Now, only 17 dots are visible.
// 
// Notice, for example, the two dots in the bottom left corner before the
// transparent paper is folded; after the fold is complete, those dots
// appear in the top left corner (at 0,0 and 0,1). Because the paper is
// transparent, the dot just below them in the result (at 0,3) remains
// visible, as it can be seen through the transparent paper.
// 
// Also notice that some dots can end up overlapping; in this case, the
// dots merge together and become a single dot.
// 
// The second fold instruction is fold along x=5, which indicates this
// line:
// 
// #.##.|#..#.
// #...#|.....
// .....|#...#
// #...#|.....
// .#.#.|#.###
// .....|.....
// .....|.....
// 
// Because this is a vertical line, fold left:
// 
// #####
// #...#
// #...#
// #...#
// #####
// .....
// .....
// 
// The instructions made a square!
// 
// The transparent paper is pretty big, so for now, focus on just
// completing the first fold. After the first fold in the example above,
// 17 dots are visible - dots that end up overlapping after the fold is
// completed count as a single dot.
// 
// How many dots are visible after completing just the first fold
// instruction on your transparent paper?
//
// --- Part Two ---
// ----------------
// 
// Finish folding the transparent paper according to the instructions.
// The manual says the code is always eight capital letters.
// 
// What code do you use to activate the infrared thermal imaging camera
// system?


#include "advent_of_code/2021/day13/day13.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/ocr.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::StatusOr<Point> ParseFold(absl::string_view fold_str) {
  absl::string_view axis_name;
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
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<Point> points;
  bool folds = false;
  for (absl::string_view line : input) {
    if (line.empty()) {
      folds = true;
      continue;
    }
    if (folds) {
      absl::StatusOr<Point> axis = ParseFold(line);
      absl::StatusOr<absl::flat_hash_set<Point>> folded =
          Fold(std::move(points), *axis);
      if (!folded.ok()) return folded.status();
      return IntReturn(folded->size());
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
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<Point> points;
  bool folds = false;
  for (absl::string_view line : input) {
    if (line.empty()) {
      folds = true;
      continue;
    }
    if (folds) {
      absl::StatusOr<Point> axis = ParseFold(line);
      absl::StatusOr<absl::flat_hash_set<Point>> folded =
          Fold(std::move(points), *axis);
      if (!folded.ok()) return folded.status();
      points = std::move(*folded);
    } else {
      Point p;
      if (!RE2::FullMatch(line, "(\\d+,\\d+)", p.Capture())) {
        return Error("Bad line: ", line);
      }
      points.insert(p);
    }
  }
  PointRectangle r;
  for (Point p : points) {
    r.ExpandInclude(p);
  }
  CharBoard b(r);
  for (Point p : points) {
    b[p] = '#';
  }
  return OCRExtract(b);
}

}  // namespace advent_of_code
