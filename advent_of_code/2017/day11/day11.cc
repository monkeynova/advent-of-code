// https://adventofcode.com/2017/day/11
//
// --- Day 11: Hex Ed ---
// ----------------------
// 
// Crossing the bridge, you've barely reached the other side of the
// stream when a program comes up to you, clearly in distress. "It's my
// child process," she says, "he's gotten lost in an infinite grid!"
// 
// Fortunately for her, you have plenty of experience with infinite
// grids.
// 
// Unfortunately for you, it's a hex grid.
// 
// The hexagons ("hexes") in this grid are aligned such that adjacent
// hexes can be found to the north, northeast, southeast, south,
// southwest, and northwest:
// 
// \ n  /
// nw +--+ ne
// /    \
// -+      +-
// \    /
// sw +--+ se
// / s  \
// 
// You have the path the child process took. Starting where he started,
// you need to determine the fewest number of steps required to reach
// him. (A "step" means to move from the hex you are in to any adjacent
// hex.)
// 
// For example:
// 
// * ne,ne,ne is 3 steps away.
// 
// * ne,ne,sw,sw is 0 steps away (back where you started).
// 
// * ne,ne,s,s is 2 steps away (se,se).
// 
// * se,sw,se,sw,sw is 3 steps away (s,s,sw).
//
// --- Part Two ---
// ----------------
// 
// How many steps away is the furthest he ever got from his starting
// position?


#include "advent_of_code/2017/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_11::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Point p = Cardinal::kOrigin;
  std::vector<absl::string_view> dirs = absl::StrSplit(input[0], ",");
  for (absl::string_view dir : dirs) {
    if (dir == "nw")
      p += Cardinal::kNorthWest;
    else if (dir == "ne")
      p += Cardinal::kNorthEast;
    else if (dir == "sw")
      p += Cardinal::kSouthWest;
    else if (dir == "se")
      p += Cardinal::kSouthEast;
    else if (dir == "n")
      p += 2 * Cardinal::kNorth;
    else if (dir == "s")
      p += 2 * Cardinal::kSouth;
    else
      return Error("Bad direction: ", dir);
  }
  return IntReturn(p.dist() / 2);
}

absl::StatusOr<std::string> Day_2017_11::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Point p = Cardinal::kOrigin;
  int max_dist = std::numeric_limits<int>::min();
  std::vector<absl::string_view> dirs = absl::StrSplit(input[0], ",");
  for (absl::string_view dir : dirs) {
    if (dir == "nw")
      p += Cardinal::kNorthWest;
    else if (dir == "ne")
      p += Cardinal::kNorthEast;
    else if (dir == "sw")
      p += Cardinal::kSouthWest;
    else if (dir == "se")
      p += Cardinal::kSouthEast;
    else if (dir == "n")
      p += 2 * Cardinal::kNorth;
    else if (dir == "s")
      p += 2 * Cardinal::kSouth;
    else
      return Error("Bad direction: ", dir);
    max_dist = std::max(max_dist, p.dist() / 2);
  }
  return IntReturn(max_dist);
}

}  // namespace advent_of_code
