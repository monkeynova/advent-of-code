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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day19_2017::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("Bad input");
  if (RE2::PartialMatch(input[0], "Ignore")) {
    input = input.subspan(1);
  }
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();
  VLOG(1) << "Board:\n" << *b;
  Point start{-1, -1};
  Point dir = Cardinal::kSouth;
  for (Point p = {0, 0}; p != Point{b->width(), 0}; p += {1, 0}) {
    if ((*b)[p] == '|') {
      if (start != Point{-1, -1}) return Error("Two starts");
      start = p;
    }
  }
  VLOG(1) << "start=" << start;
  Point p = start;
  std::string sequence;
  while (true) {
    VLOG(2) << "@" << p << " D:" << dir;
    Point next = p + dir;
    if (!b->OnBoard(next) || b->at(next) == ' ') {
      dir = dir.rotate_left();
      next = p + dir;
      if (!b->OnBoard(next) || b->at(next) == ' ') {
        dir = dir.rotate_right().rotate_right();
        next = p + dir;
      }
      if (!b->OnBoard(next) || b->at(next) == ' ') {
        if (b->at(p) == '+') return Error("Erroneously marked end @", p.DebugString());
        break;
      }
      if (b->at(p) != '+') return Error("Turning at non-intersection @", p.DebugString());
    }
    if (b->at(next) == '+' || b->at(next) == '|' || b->at(next) == '-') {
      // Still on path. Keep going.
      // Don't verify direction because '+' is only used for turns and
      // intersections exist with one of '|' or '-'.
    } else if (b->at(next) >= 'A' && b->at(next) <= 'Z') {
      VLOG(1) << "Appending: " << b->at(next) << " to " << sequence;
      sequence.append(1, b->at(next));
    } else {
      return Error("Bad token @", next.DebugString());
    }
    p = next;
  }
  return std::vector<std::string>{sequence};
}

absl::StatusOr<std::vector<std::string>> Day19_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
