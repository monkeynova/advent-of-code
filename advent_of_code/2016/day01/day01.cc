// https://adventofcode.com/2016/day/1
//
// --- Day 1: No Time for a Taxicab ---
// ------------------------------------
//
// Santa's sleigh uses a very high-precision clock to guide its
// movements, and the clock's oscillator is regulated by stars.
// Unfortunately, the stars have been stolen... by the Easter Bunny. To
// save Christmas, Santa needs you to retrieve all fifty stars by
// December 25th.
//
// Collect stars by solving puzzles. Two puzzles will be made available
// on each day in the Advent calendar; the second puzzle is unlocked when
// you complete the first. Each puzzle grants one star. Good luck!
//
// You're airdropped near Easter Bunny Headquarters in a city somewhere.
// "Near", unfortunately, is as close as you can get - the instructions
// on the Easter Bunny Recruiting Document the Elves intercepted start
// here, and nobody had time to work them out further.
//
// The Document indicates that you should start at the given coordinates
// (where you just landed) and face North. Then, follow the provided
// sequence: either turn left (L) or right (R) 90 degrees, then walk
// forward the given number of blocks, ending at a new intersection.
//
// There's no time to follow such ridiculous instructions on foot,
// though, so you take a moment and work out the destination. Given that
// you can only walk on the street grid of the city, how far is the
// shortest path to the destination?
//
// For example:
//
// * Following R2, L3 leaves you 2 blocks East and 3 blocks North, or 5
// blocks away.
//
// * R2, R2, R2 leaves you 2 blocks due South of your starting
// position, which is 2 blocks away.
//
// * R5, L5, R5, R3 leaves you 12 blocks away.
//
// How many blocks away is Easter Bunny HQ?
//
// --- Part Two ---
// ----------------
//
// Then, you notice the instructions continue on the back of the
// Recruiting Document. Easter Bunny HQ is actually at the first location
// you visit twice.
//
// For example, if your instructions are R8, R4, R4, R8, the first
// location you visit twice is 4 blocks away, due East.
//
// How many blocks away is the first location you visit twice?

#include "advent_of_code/2016/day01/day01.h"

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

absl::StatusOr<std::string> Day_2016_01::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::vector<absl::string_view> instructions = absl::StrSplit(input[0], ", ");
  Point p = {0, 0};
  Point heading = Cardinal::kNorth;
  for (absl::string_view ins : instructions) {
    int dist;
    if (RE2::FullMatch(ins, "L(\\d+)", &dist)) {
      heading = heading.rotate_left();
    } else if (RE2::FullMatch(ins, "R(\\d+)", &dist)) {
      heading = heading.rotate_right();
    } else {
      return Error("Bad instruction: ", ins);
    }
    p += dist * heading;
  }
  return IntReturn(p.dist());
}

absl::StatusOr<std::string> Day_2016_01::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::vector<absl::string_view> instructions = absl::StrSplit(input[0], ", ");
  Point p = {0, 0};
  Point heading = Cardinal::kNorth;
  absl::flat_hash_set<Point> hist = {p};
  for (absl::string_view ins : instructions) {
    int dist;
    if (RE2::FullMatch(ins, "L(\\d+)", &dist)) {
      heading = heading.rotate_left();
    } else if (RE2::FullMatch(ins, "R(\\d+)", &dist)) {
      heading = heading.rotate_right();
    } else {
      return Error("Bad instruction: ", ins);
    }
    for (int i = 0; i < dist; ++i) {
      p += heading;
      if (hist.contains(p)) return IntReturn(p.dist());
      hist.insert(p);
    }
  }
  return Error("No duplicate location");
}

}  // namespace advent_of_code
