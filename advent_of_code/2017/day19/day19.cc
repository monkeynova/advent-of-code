// http://adventofcode.com/2017/day/19

#include "advent_of_code/2017/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
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
        if (b[p] == '+')
          return Error("Erroneously marked end @", p);
        break;
      }
      if (b[p] != '+')
        return Error("Turning at non-intersection @", p);
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
