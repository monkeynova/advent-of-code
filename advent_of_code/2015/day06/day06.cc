// https://adventofcode.com/2015/day/6
//
// --- Day 6: Probably a Fire Hazard ---
// -------------------------------------
//
// Because your neighbors keep defeating you in the holiday house
// decorating contest year after year, you've decided to deploy one
// million lights in a 1000x1000 grid.
//
// Furthermore, because you've been especially nice this year, Santa has
// mailed you instructions on how to display the ideal lighting
// configuration.
//
// Lights in your grid are numbered from 0 to 999 in each direction; the
// lights at each corner are at 0,0, 0,999, 999,999, and 999,0. The
// instructions include whether to turn on, turn off, or toggle various
// inclusive ranges given as coordinate pairs. Each coordinate pair
// represents opposite corners of a rectangle, inclusive; a coordinate
// pair like 0,0 through 2,2 therefore refers to 9 lights in a 3x3
// square. The lights all start turned off.
//
// To defeat your neighbors this year, all you have to do is set up your
// lights by doing the instructions Santa sent you in order.
//
// For example:
//
// * turn on 0,0 through 999,999 would turn on (or leave on) every
// light.
//
// * toggle 0,0 through 999,0 would toggle the first line of 1000
// lights, turning off the ones that were on, and turning on the ones
// that were off.
//
// * turn off 499,499 through 500,500 would turn off (or leave off) the
// middle four lights.
//
// After following the instructions, how many lights are lit?
//
// --- Part Two ---
// ----------------
//
// You just finish implementing your winning light pattern when you
// realize you mistranslated Santa's message from Ancient Nordic Elvish.
//
// The light grid you bought actually has individual brightness controls;
// each light can have a brightness of zero or more. The lights all start
// at zero.
//
// The phrase turn on actually means that you should increase the
// brightness of those lights by 1.
//
// The phrase turn off actually means that you should decrease the
// brightness of those lights by 1, to a minimum of zero.
//
// The phrase toggle actually means that you should increase the
// brightness of those lights by 2.
//
// What is the total brightness of all lights combined after following
// Santa's instructions?
//
// For example:
//
// * turn on 0,0 through 0,0 would increase the total brightness by 1.
//
// * toggle 0,0 through 999,999 would increase the total brightness by
// 2000000.

#include "advent_of_code/2015/day06/day06.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_06::Part1(
    absl::Span<absl::string_view> input) const {
  CharBoard yard(1000, 1000);

  for (absl::string_view str : input) {
    PointRectangle r;
    absl::string_view cmd;
    if (!RE2::FullMatch(str, "(.*) (\\d+,\\d+) through (\\d+,\\d+)", &cmd,
                        r.min.Capture(), r.max.Capture())) {
      return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
    }
    VLOG(1) << cmd << ": " << r;
    if (cmd == "turn on") {
      for (Point p : r) {
        VLOG(2) << p;
        yard[p] = '#';
      }
    } else if (cmd == "turn off") {
      for (Point p : r) {
        VLOG(2) << p;
        yard[p] = '.';
      }
    } else if (cmd == "toggle") {
      for (Point p : r) {
        VLOG(2) << p << ": " << yard[p];
        yard[p] = yard[p] == '#' ? '.' : '#';
      }
    } else {
      return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
    }
    VLOG(1) << "  on: " << yard.CountOn();
  }

  return IntReturn(yard.CountOn());
}

absl::StatusOr<std::string> Day_2015_06::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<std::vector<int>> yard;
  std::vector<int> empty_row;
  empty_row.resize(1000);
  for (int i = 0; i < 1000; ++i) {
    yard.push_back(empty_row);
  }

  for (absl::string_view str : input) {
    Point min;
    Point max;
    absl::string_view cmd;
    if (!RE2::FullMatch(str, "(.*) (\\d+,\\d+) through (\\d+,\\d+)", &cmd,
                        min.Capture(), max.Capture())) {
      return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
    }
    if (cmd == "turn on") {
      for (int y = min.y; y <= max.y; ++y) {
        for (int x = min.x; x <= max.x; ++x) {
          ++yard[y][x];
        }
      }
    } else if (cmd == "turn off") {
      for (int y = min.y; y <= max.y; ++y) {
        for (int x = min.x; x <= max.x; ++x) {
          if (--yard[y][x] <= 0) yard[y][x] = 0;
        }
      }
    } else if (cmd == "toggle") {
      for (int y = min.y; y <= max.y; ++y) {
        for (int x = min.x; x <= max.x; ++x) {
          yard[y][x] += 2;
        }
      }
    } else {
      return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
    }
  }

  int total_brightness = 0;
  for (const std::vector<int>& row : yard) {
    for (int b : row) {
      total_brightness += b;
    }
  }

  return IntReturn(total_brightness);
}

}  // namespace advent_of_code
