// https://adventofcode.com/2015/day/3
//
// --- Day 3: Perfectly Spherical Houses in a Vacuum ---
// -----------------------------------------------------
// 
// Santa is delivering presents to an infinite two-dimensional grid of
// houses.
// 
// He begins by delivering a present to the house at his starting
// location, and then an elf at the North Pole calls him via radio and
// tells him where to move next. Moves are always exactly one house to
// the north (^), south (v), east (>), or west (<). After each move, he
// delivers another present to the house at his new location.
// 
// However, the elf back at the north pole has had a little too much
// eggnog, and so his directions are a little off, and Santa ends up
// visiting some houses more than once. How many houses receive at least
// one present?
// 
// For example:
// 
// * > delivers presents to 2 houses: one at the starting location, and
// one to the east.
// 
// * ^>v< delivers presents to 4 houses in a square, including twice to
// the house at his starting/ending location.
// 
// * ^v^v^v^v^v delivers a bunch of presents to some very lucky
// children at only 2 houses.
//
// --- Part Two ---
// ----------------
// 
// The next year, to speed up the process, Santa creates a robot version
// of himself, Robo-Santa, to deliver presents with him.
// 
// Santa and Robo-Santa start at the same location (delivering two
// presents to the same starting house), then take turns moving based on
// instructions from the elf, who is eggnoggedly reading from the same
// script as the previous year.
// 
// This year, how many houses receive at least one present?
// 
// For example:
// 
// * ^v delivers presents to 3 houses, because Santa goes north, and
// then Robo-Santa goes south.
// 
// * ^>v< now delivers presents to 3 houses, and Santa and Robo-Santa
// end up back where they started.
// 
// * ^v^v^v^v^v now delivers presents to 11 houses, with Santa going
// one direction and Robo-Santa going the other.


#include "advent_of_code/2015/day03/day03.h"

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

absl::StatusOr<std::string> Day_2015_03::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  absl::flat_hash_map<Point, int> visit;
  Point p{0, 0};
  for (char c : input[0]) {
    ++visit[p];
    switch (c) {
      case '^':
        p += Cardinal::kNorth;
        break;
      case 'v':
        p += Cardinal::kSouth;
        break;
      case '<':
        p += Cardinal::kWest;
        break;
      case '>':
        p += Cardinal::kEast;
        break;
      default:
        return absl::InvalidArgumentError("Bad direction");
    }
  }
  return IntReturn(visit.size());
}

absl::StatusOr<std::string> Day_2015_03::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  if (input[0].size() % 2 != 0)
    return absl::InvalidArgumentError("Bad input length");
  absl::flat_hash_map<Point, int> visit;
  Point p1{0, 0};
  Point p2{0, 0};
  for (int i = 0; i < input[0].size(); i += 2) {
    ++visit[p1];
    ++visit[p2];
    switch (input[0][i]) {
      case '^':
        p1 += Cardinal::kNorth;
        break;
      case 'v':
        p1 += Cardinal::kSouth;
        break;
      case '<':
        p1 += Cardinal::kWest;
        break;
      case '>':
        p1 += Cardinal::kEast;
        break;
      default:
        return absl::InvalidArgumentError("Bad direction");
    }
    switch (input[0][i + 1]) {
      case '^':
        p2 += Cardinal::kNorth;
        break;
      case 'v':
        p2 += Cardinal::kSouth;
        break;
      case '<':
        p2 += Cardinal::kWest;
        break;
      case '>':
        p2 += Cardinal::kEast;
        break;
      default:
        return absl::InvalidArgumentError("Bad direction");
    }
  }
  return IntReturn(visit.size());
}

}  // namespace advent_of_code
