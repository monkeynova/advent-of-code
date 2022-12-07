// https://adventofcode.com/2017/day/19
//
// --- Day 19: A Series of Tubes ---
// ---------------------------------
//
// Somehow, a network packet got lost and ended up here. It's trying to
// follow a routing diagram (your puzzle input), but it's confused about
// where to go.
//
// Its starting point is just off the top of the diagram. Lines (drawn
// with |, -, and +) show the path it needs to take, starting by going
// down onto the only line connected to the top of the diagram. It needs
// to follow this path until it reaches the end (located somewhere within
// the diagram) and stop there.
//
// Sometimes, the lines cross over each other; in these cases, it needs
// to continue going the same direction, and only turn left or right when
// there's no other option. In addition, someone has left letters on the
// line; these also don't change its direction, but it can use them to
// keep track of where it's been. For example:
//
// |
// |  +--+
// A  |  C
// F---|----E|--+
// |  |  |  D
// +B-+  +--+
//
//
// Given this diagram, the packet needs to take the following path:
//
// * Starting at the only line touching the top of the diagram, it must
// go down, pass through A, and continue onward to the first +.
//
// * Travel right, up, and right, passing through B in the process.
//
// * Continue down (collecting C), right, and up (collecting D).
//
// * Finally, go all the way left through E and stopping at F.
//
// Following the path to the end, the letters it sees on its path are
// ABCDEF.
//
// The little packet looks up at you, hoping you can help it find the
// way. What letters will it see (in the order it would see them) if it
// follows the path? (The routing diagram is very wide; make sure you
// view it without line wrapping.)
//
// --- Part Two ---
// ----------------
//
// The packet is curious how many steps it needs to go.
//
// For example, using the same routing diagram from the example above...
//
// |
// |  +--+
// A  |  C
// F---|--|-E---+
// |  |  |  D
// +B-+  +--+
//
//
// ...the packet would go:
//
// * 6 steps down (including the first line at the top of the diagram).
//
// * 3 steps right.
//
// * 4 steps up.
//
// * 3 steps right.
//
// * 4 steps down.
//
// * 3 steps right.
//
// * 2 steps up.
//
// * 13 steps left (including the F it stops on).
//
// This would result in a total of 38 steps.
//
// How many steps does the packet need to go?

#include "advent_of_code/2017/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct WalkRet {
  std::string sequence;
  int num_steps = 0;
};

absl::StatusOr<WalkRet> WalkBoard(const CharBoard& b) {
  VLOG(1) << "Board:\n" << b;
  Point start{-1, -1};
  Point dir = Cardinal::kSouth;
  for (Point p = {0, 0}; p != Point{b.width(), 0}; p += {1, 0}) {
    if (b[p] == '|') {
      if (start != Point{-1, -1}) return Error("Two starts");
      start = p;
    }
  }
  VLOG(1) << "start=" << start;
  Point p = start;
  WalkRet ret;
  while (true) {
    ++ret.num_steps;
    VLOG(2) << "@" << p << " D:" << dir;
    Point next = p + dir;
    if (!b.OnBoard(next) || b[next] == ' ') {
      dir = dir.rotate_left();
      next = p + dir;
      if (!b.OnBoard(next) || b[next] == ' ') {
        dir = dir.rotate_right().rotate_right();
        next = p + dir;
      }
      if (!b.OnBoard(next) || b[next] == ' ') {
        if (b[p] == '+') return Error("Erroneously marked end @", p);
        break;
      }
      if (b[p] != '+') return Error("Turning at non-intersection @", p);
    }
    if (b[next] == '+' || b[next] == '|' || b[next] == '-') {
      // Still on path. Keep going.
      // Don't verify direction because '+' is only used for turns and
      // intersections exist with one of '|' or '-'.
    } else if (b[next] >= 'A' && b[next] <= 'Z') {
      VLOG(1) << "Appending: " << b[next] << " to " << ret.sequence;
      ret.sequence.append(1, b[next]);
    } else {
      return Error("Bad token @", next);
    }
    p = next;
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_19::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("Bad input");
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();
  absl::StatusOr<WalkRet> walk_ret = WalkBoard(*b);
  if (!walk_ret.ok()) return walk_ret.status();
  return walk_ret->sequence;
}

absl::StatusOr<std::string> Day_2017_19::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("Bad input");
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();
  absl::StatusOr<WalkRet> walk_ret = WalkBoard(*b);
  if (!walk_ret.ok()) return walk_ret.status();
  return IntReturn(walk_ret->num_steps);
}

}  // namespace advent_of_code
