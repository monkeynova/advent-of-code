// http://adventofcode.com/2024/day/4

#include "advent_of_code/2024/day04/day04.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_04::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  int xmas_count = 0;
  for (Point p : b.range()) {
    if (b[p] != 'X') continue;
    for (Point d : Cardinal::kEightDirs) {
      Point t = p + d;
      if (!b.OnBoard(t) || b[t] != 'M') continue;
      t += d;
      if (!b.OnBoard(t) || b[t] != 'A') continue;
      t += d;
      if (!b.OnBoard(t) || b[t] != 'S') continue;
      ++xmas_count;
    }
  }
  return AdventReturn(xmas_count);
}

absl::StatusOr<std::string> Day_2024_04::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  int xmas_count = 0;
  PointRectangle range = b.range();
  range.min += Cardinal::kSouthEast;
  range.max += Cardinal::kNorthWest;
  for (Point p : range) {
    if (b[p] != 'A') continue;
    {
      Point t1 = p + Cardinal::kNorthEast;
      if (b[t1] != 'M' && b[t1] != 'S') continue;
      Point t2 = p + Cardinal::kSouthWest;
      if (b[t2] != 'M' && b[t2] != 'S') continue;
      if (b[t1] == b[t2]) continue;
    }
    {
      Point t1 = p + Cardinal::kNorthWest;
      if (b[t1] != 'M' && b[t1] != 'S') continue;
      Point t2 = p + Cardinal::kSouthEast;
      if (b[t2] != 'M' && b[t2] != 'S') continue;
      if (b[t1] == b[t2]) continue;
    }

    ++xmas_count;
  }
  return AdventReturn(xmas_count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/4,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_04()); });

}  // namespace advent_of_code
