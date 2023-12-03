// http://adventofcode.com/2023/day/3

#include "advent_of_code/2023/day03/day03.h"

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
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

bool IsDigit(char c) {
  return c >= '0' && c <= '9';
}

}  // namespace

absl::StatusOr<std::string> Day_2023_03::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  int sum = 0;
  std::vector<bool> used(b.range().Area(), false);
  auto point_idx = [&](Point p) {
    return p.y * b.width() + p.x;
  };
  auto valid_digit = [&](Point p) {
    return b.OnBoard(p) && IsDigit(b[p]) && !used[point_idx(p)];
  };
  for (auto [p, c] : b) {
    if (c == '.' || IsDigit(c)) continue;
    for (Point dir : Cardinal::kEightDirs) {
      Point t = p + dir;
      if (!valid_digit(t)) continue;
      Point min = t;
      for (; valid_digit(min); min += Cardinal::kWest) {
        used[point_idx(min)] = true;
      }
      min += Cardinal::kEast;
      Point max = t;
      for (max += Cardinal::kEast; valid_digit(max); max += Cardinal::kEast) {
        used[point_idx(max)] = true;
      }
      int num = 0;
      for (Point d = min; d != max; ++d.x) {
        num = num * 10 + b[d] - '0';
      }
      sum += num;
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2023_03::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  int64_t sum = 0;
  std::vector<bool> used(b.range().Area(), false);
  auto point_idx = [&](Point p) {
    return p.y * b.width() + p.x;
  };
  auto valid_digit = [&](Point p) {
    return b.OnBoard(p) && IsDigit(b[p]) && !used[point_idx(p)];
  };
  for (auto [p, c] : b) {
    if (c != '*') continue;
    std::vector<int64_t> adjacent;
    for (Point dir : Cardinal::kEightDirs) {
      Point t = p + dir;
      if (!valid_digit(t)) continue;
      Point min = t;
      for (; valid_digit(min); min += Cardinal::kWest) {
        used[point_idx(min)] = true;
      }
      min += Cardinal::kEast;
      Point max = t;
      for (max += Cardinal::kEast; valid_digit(max); max += Cardinal::kEast) {
        used[point_idx(max)] = true;
      }
      int num = 0;
      for (Point d = min; d != max; ++d.x) {
        num = num * 10 + b[d] - '0';
      }
      adjacent.push_back(num);
    }
    if (adjacent.size() == 2) {
      sum += adjacent[0] * adjacent[1];
    }
  }
  return AdventReturn(sum);
}

}  // namespace advent_of_code
