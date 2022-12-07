// https://adventofcode.com/2021/day/5
//
// --- Day 5: Hydrothermal Venture ---
// -----------------------------------
//
// You come across a field of hydrothermal vents on the ocean floor!
// These vents constantly produce large, opaque clouds, so it would be
// best to avoid them if possible.
//
// They tend to form in lines; the submarine helpfully produces a list of
// nearby lines of vents (your puzzle input) for you to review. For
// example:
//
// 0,9 -> 5,9
// 8,0 -> 0,8
// 9,4 -> 3,4
// 2,2 -> 2,1
// 7,0 -> 7,4
// 6,4 -> 2,0
// 0,9 -> 2,9
// 3,4 -> 1,4
// 0,0 -> 8,8
// 5,5 -> 8,2
//
// Each line of vents is given as a line segment in the format x1,y1 ->
// x2,y2 where x1,y1 are the coordinates of one end the line segment and
// x2,y2 are the coordinates of the other end. These line segments
// include the points at both ends. In other words:
//
// * An entry like 1,1 -> 1,3 covers points 1,1, 1,2, and 1,3.
//
// * An entry like 9,7 -> 7,7 covers points 9,7, 8,7, and 7,7.
//
// For now, only consider horizontal and vertical lines: lines where
// either x1 = x2 or y1 = y2.
//
// So, the horizontal and vertical lines from the above list would
// produce the following diagram:
//
// .......1..
// ..1....1..
// ..1....1..
// .......1..
// .112111211
// ..........
// ..........
// ..........
// ..........
// 222111....
//
// In this diagram, the top left corner is 0,0 and the bottom right
// corner is 9,9. Each position is shown as the number of lines which
// cover that point or . if no line covers that point. The top-left pair
// of 1s, for example, comes from 2,2 -> 2,1; the very bottom row is
// formed by the overlapping lines 0,9 -> 5,9 and 0,9 -> 2,9.
//
// To avoid the most dangerous areas, you need to determine the number of
// points where at least two lines overlap. In the above example, this is
// anywhere in the diagram with a 2 or larger - a total of 5 points.
//
// Consider only horizontal and vertical lines. At how many points do at
// least two lines overlap?
//
// --- Part Two ---
// ----------------
//
// Unfortunately, considering only horizontal and vertical lines doesn't
// give you the full picture; you need to also consider diagonal lines.
//
// Because of the limits of the hydrothermal vent mapping system, the
// lines in your list will only ever be horizontal, vertical, or a
// diagonal line at exactly 45 degrees. In other words:
//
// * An entry like 1,1 -> 3,3 covers points 1,1, 2,2, and 3,3.
//
// * An entry like 9,7 -> 7,9 covers points 9,7, 8,8, and 7,9.
//
// Considering all lines from the above example would now produce the
// following diagram:
//
// 1.1....11.
// .111...2..
// ..2.1.111.
// ...1.2.2..
// .112313211
// ...1.2....
// ..1...1...
// .1.....1..
// 1.......1.
// 222111....
//
// You still need to determine the number of points where at least two
// lines overlap. In the above example, this is still anywhere in the
// diagram with a 2 or larger - now a total of 12 points.
//
// Consider all of the lines. At how many points do at least two lines
// overlap?

#include "advent_of_code/2021/day05/day05.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_05::Part1(
    absl::Span<absl::string_view> input) const {
  RE2 line_re("(\\d+,\\d+) -> (\\d+,\\d+)");
  absl::flat_hash_map<Point, int64_t> counts;
  for (absl::string_view line : input) {
    Point p1;
    Point p2;
    if (!RE2::FullMatch(line, line_re, p1.Capture(), p2.Capture())) {
      return Error("Bad line: ", line);
    }
    Point dir = (p2 - p1).min_step();
    if (dir.dist() > 1) continue;
    for (Point p = p1; p != p2; p += dir) {
      ++counts[p];
    }
    ++counts[p2];
  }
  int64_t ret = 0;
  for (const auto& [_, count] : counts) {
    if (count > 1) ++ret;
  }
  return IntReturn(ret);
}

absl::StatusOr<std::string> Day_2021_05::Part2(
    absl::Span<absl::string_view> input) const {
  RE2 line_re("(\\d+,\\d+) -> (\\d+,\\d+)");
  absl::flat_hash_map<Point, int64_t> counts;
  for (absl::string_view line : input) {
    Point p1;
    Point p2;
    if (!RE2::FullMatch(line, line_re, p1.Capture(), p2.Capture())) {
      return Error("Bad line: ", line);
    }
    Point dir = (p2 - p1).min_step();
    if (dir.dist() > 2) {
      return Error("Found non-0/45/90 degree slope: ", dir);
    }
    for (Point p = p1; p != p2; p += dir) {
      ++counts[p];
    }
    ++counts[p2];
  }
  int64_t ret = 0;
  for (const auto& [_, count] : counts) {
    if (count > 1) ++ret;
  }
  return IntReturn(ret);
}

}  // namespace advent_of_code
