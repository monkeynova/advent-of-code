#include "advent_of_code/2018/day03/day03.h"

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

absl::StatusOr<std::vector<std::string>> Day03_2018::Part1(
    absl::Span<absl::string_view> input) const {
  // #1372 @ 119,13: 29x16
  CharBoard board(1'000, 1'000);
  for (Point p : board.range()) board[p] = '.';
  int dupes = 0;
  for (absl::string_view row : input) {
    PointRectangle r;
    if (!RE2::FullMatch(row, "#\\d+ @ (\\d+),(\\d+): (\\d+)x(\\d+)",
                        &r.min.x, &r.min.y, &r.max.x, &r.max.y)) {
      return Error("Bad input: ", row);
    }
    r.max.x += r.min.x - 1;
    r.max.y += r.min.y - 1;
    if (!board.OnBoard(r.min)) return Error("Min off board");
    if (!board.OnBoard(r.max)) return Error("Max off board");
    for (Point p : r) {
      if (board[p] == '+') {
        ++dupes;
        board[p] = '#';
      } else if (board[p] == '#') {
        // Already counted.
      } else {
        board[p] = '+';
      }
    }
  }
  return IntReturn(dupes);
}

absl::StatusOr<std::vector<std::string>> Day03_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
