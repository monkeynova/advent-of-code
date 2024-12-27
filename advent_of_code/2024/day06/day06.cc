// http://adventofcode.com/2024/day/6

#include "advent_of_code/2024/day06/day06.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/fast_board.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_06::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  FastBoard fb(b);
  FastBoard::Point p = fb.FindUnique('^');
  FastBoard::PointMap<bool> visited(fb, false);
  FastBoard::Dir dir = FastBoard::kNorth;

  int visit_count = 1;
  visited.Set(p, true);
  for (p = fb.Add(p, dir); fb.OnBoard(p); p = fb.Add(p, dir)) {
    if (fb[p] == '#') {
      p = fb.Add(p, FastBoard::kReverse[dir]);
      dir = FastBoard::kRotateRight[dir];
    }
    if (!visited.Get(p)) {
      ++visit_count;
      visited.Set(p, true);
    }
  }

  return AdventReturn(visit_count);
}

absl::StatusOr<std::string> Day_2024_06::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  FastBoard fb(b);
  FastBoard::Point start = fb.FindUnique('^');
  FastBoard::PointMap<bool> blockable(fb, false);
  FastBoard::PointDirMap<bool> history(fb, false);
  FastBoard::Dir dir = FastBoard::kNorth;

  int make_loop = 0;
  blockable.Set(start, true);
  history.Set(start, dir, true);
  FastBoard::PointDirMap<bool> history2(fb, false);
  for (FastBoard::Point p = fb.Add(start, dir); fb.OnBoard(p); p = fb.Add(p, dir)) {
    if (fb[p] == '#') {
      p = fb.Add(p, FastBoard::kReverse[dir]);
      dir = FastBoard::kRotateRight[dir];
    } else if (!blockable.Get(p)) {
      history2 = history;
      FastBoard::Dir dir2 = FastBoard::kRotateRight[dir];
      for (FastBoard::Point p2 = fb.Add(p, FastBoard::kReverse[dir]); fb.OnBoard(p2); p2 = fb.Add(p2, dir2)) {
        if (fb[p2] == '#' || p == p2) {
          p2 = fb.Add(p2, FastBoard::kReverse[dir2]);
          dir2 = FastBoard::kRotateRight[dir2];
          continue;
        }
        if (history2.Get(p2, dir2)) {
          ++make_loop;
          break;
        }
        history2.Set(p2, dir2, true);
      }
    }
    blockable.Set(p, true);
    history.Set(p, dir, true);
  }

  return AdventReturn(make_loop);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/6,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_06()); });

}  // namespace advent_of_code
