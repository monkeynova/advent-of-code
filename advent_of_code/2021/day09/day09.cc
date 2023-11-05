#include "advent_of_code/2021/day09/day09.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point_walk.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

std::vector<Point> FindLow(const CharBoard& board) {
  std::vector<Point> ret;
  for (Point p : board.range()) {
    bool low = true;
    for (Point d : Cardinal::kFourDirs) {
      Point p2 = p + d;
      if (!board.OnBoard(p2)) continue;
      if (board[p] >= board[p2]) {
        low = false;
        break;
      }
    }
    if (low) {
      ret.push_back(p);
    }
  }
  return ret;
}

int64_t BasinSize(const CharBoard& board, Point p) {
  return PointWalk({
                       .start = p,
                       .is_good =
                           [&](Point test, int) {
                             return board.OnBoard(test) && board[test] != '9';
                           },
                       .is_final = [](Point, int) { return false; },
                   })
      .FindReachable()
      .size();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_09::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input);
  if (!board.ok()) return board.status();

  std::vector<Point> basins = FindLow(*board);
  int64_t sum_risk = 0;
  for (Point p : basins) {
    sum_risk += (*board)[p] + 1 - '0';
  }

  return AdventReturn(sum_risk);
}

absl::StatusOr<std::string> Day_2021_09::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input);
  if (!board.ok()) return board.status();

  std::vector<Point> basins = FindLow(*board);
  if (basins.size() < 3) return Error("Not enough basins");

  std::vector<int64_t> sizes;
  for (Point start : basins) {
    sizes.push_back(BasinSize(*board, start));
  }
  absl::c_sort(sizes, [](int64_t a, int64_t b) { return b < a; });

  return AdventReturn(sizes[0] * sizes[1] * sizes[2]);
}

}  // namespace advent_of_code
