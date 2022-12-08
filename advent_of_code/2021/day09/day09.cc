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
  absl::flat_hash_set<Point> history = {p};
  for (std::deque<Point> frontier = {p}; !frontier.empty();
       frontier.pop_front()) {
    Point cur = frontier.front();
    for (Point d : Cardinal::kFourDirs) {
      Point n = cur + d;
      if (!board.OnBoard(n)) continue;
      if (board[n] == '9') continue;
      if (history.contains(n)) continue;
      history.insert(n);
      frontier.push_back(n);
    }
  }
  return history.size();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_09::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();

  std::vector<Point> basins = FindLow(*board);
  int64_t sum_risk = 0;
  for (Point p : basins) {
    sum_risk += (*board)[p] + 1 - '0';
  }

  return IntReturn(sum_risk);
}

absl::StatusOr<std::string> Day_2021_09::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();

  std::vector<Point> basins = FindLow(*board);
  if (basins.size() < 3) return Error("Not enough basins");

  std::vector<int64_t> sizes;
  for (Point start : basins) {
    sizes.push_back(BasinSize(*board, start));
  }
  absl::c_sort(sizes, [](int64_t a, int64_t b) { return b < a; });

  return IntReturn(sizes[0] * sizes[1] * sizes[2]);
}

}  // namespace advent_of_code
