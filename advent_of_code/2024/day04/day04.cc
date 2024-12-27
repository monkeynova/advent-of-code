// http://adventofcode.com/2024/day/4

#include "advent_of_code/2024/day04/day04.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_04::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  absl::flat_hash_set<Point> x_points = b.Find('X');
  int xmas_count = 0;
  for (Point p : x_points) {
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
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  absl::flat_hash_set<Point> x_points = b.Find('A');
  int xmas_count = 0;
  for (Point p : x_points) {
    {
      Point t1 = p + Cardinal::kNorthEast;
      if (!b.OnBoard(t1) || (b[t1] != 'M' && b[t1] != 'S')) continue;
      Point t2 = p + Cardinal::kSouthWest;
      if (!b.OnBoard(t2) || (b[t2] != 'M' && b[t2] != 'S') || b[t1] == b[t2]) continue;
    }
    {
      Point t1 = p + Cardinal::kNorthWest;
      if (!b.OnBoard(t1) || (b[t1] != 'M' && b[t1] != 'S')) continue;
      Point t2 = p + Cardinal::kSouthEast;
      if (!b.OnBoard(t2) || (b[t2] != 'M' && b[t2] != 'S') || b[t1] == b[t2]) continue;
    }

    ++xmas_count;
  }
  return AdventReturn(xmas_count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/4,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_04()); });

}  // namespace advent_of_code