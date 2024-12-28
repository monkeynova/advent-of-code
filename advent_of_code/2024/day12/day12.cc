// http://adventofcode.com/2024/day/12

#include "advent_of_code/2024/day12/day12.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

int64_t ScoreAndMarkRegion(
    const FastBoard& b, FastBoard::Point p, FastBoard::PointMap<bool>& seen) {
  int area = 1;
  int perimeter = 0;

  seen.Set(p, true);
  std::vector<FastBoard::Point> frontier = {p};
  while (!frontier.empty()) {
    std::vector<FastBoard::Point> new_frontier;
    for (FastBoard::Point p : frontier) {
      for (FastBoard::Dir d : FastBoard::kFourDirs) {
        FastBoard::Point t = b.Add(p, d);
        if (b.OnBoard(t) && b[p] == b[t]) {
          if (!seen.Get(t)) {
            new_frontier.push_back(t);
            seen.Set(t, true);
            ++area;
          }
        } else {
          ++perimeter;
        }
      }
    }
    frontier = std::move(new_frontier);
  }

  return area * perimeter;
}

int64_t Score2AndMarkRegion(
    const FastBoard& b, FastBoard::Point p, FastBoard::PointMap<bool>& seen) {
  int area = 1;
  int sides = 0;

  seen.Set(p, true);
  std::vector<FastBoard::Point> frontier = {p};
  while (!frontier.empty()) {
    std::vector<FastBoard::Point> new_frontier;
    for (FastBoard::Point p : frontier) {
      for (FastBoard::Dir d : FastBoard::kFourDirs) {
        FastBoard::Point t = b.Add(p, d);
        if (b.OnBoard(t) && b[p] == b[t]) {
          if (!seen.Get(t)) {
            new_frontier.push_back(t);
            seen.Set(t, true);
            ++area;
          }
        } else {
          // We've got a boundary, look to see if the boundary continues one
          // block to the left, in which case don't count it as a new side.
          FastBoard::Dir ld = FastBoard::kRotateLeft[d];
          FastBoard::Point t2 = b.Add(p, ld);
          FastBoard::Point t3 = b.Add(t2, d);

          if (!b.OnBoard(t2) || b[p] != b[t2] ||
              (b.OnBoard(t3) && b[p] == b[t3])) {
            ++sides;
          }
        }
      }
    }
    frontier = std::move(new_frontier);
  }

  return area * sides;
}

}  // namespace

absl::StatusOr<std::string> Day_2024_12::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  FastBoard fb(b);
  FastBoard::PointMap<bool> used(fb, false);
  int total_score = 0;
  for (FastBoard::Point fp : fb) {
    if (used.Get(fp)) continue;
    total_score += ScoreAndMarkRegion(fb, fp, used);
  }
  return AdventReturn(total_score);
}

absl::StatusOr<std::string> Day_2024_12::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  FastBoard fb(b);
  FastBoard::PointMap<bool> used(fb, false);
  int total_score = 0;
  for (FastBoard::Point fp : fb) {
    if (used.Get(fp)) continue;
    total_score += Score2AndMarkRegion(fb, fp, used);
  }
  return AdventReturn(total_score);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/12,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_12()); });

}  // namespace advent_of_code
