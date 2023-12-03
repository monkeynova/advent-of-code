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
  absl::flat_hash_set<Point> used;
  for (Point p : b.range()) {
    bool has_symbol = false;
    int cur_num = 0;
    for (Point p2 = p; !used.contains(p2) && b.OnBoard(p2) && IsDigit(b[p2]); ++p2.x) {
      used.insert(p2);
      cur_num = cur_num * 10 + b[p2] - '0';
      for (Point d : Cardinal::kEightDirs) {
        Point t = p2 + d;
        if (b.OnBoard(t) && b[t] != '.' && !IsDigit(b[t])) {
          has_symbol = true;
        }
      }
    }
    if (has_symbol) {
      sum += cur_num;
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2023_03::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  int64_t sum = 0;
  absl::flat_hash_set<Point> used;
  for (Point p : b.range()) {
    if (b[p] != '*') continue;
    std::vector<int64_t> adjacent;
    for (Point dir : Cardinal::kEightDirs) {
      Point t = p + dir;
      if (used.contains(t) || !IsDigit(b[t])) continue;
      Point min = t;
      while (!used.contains(min) && b.OnBoard(min) && IsDigit(b[min])) {
        used.insert(min);
        --min.x;
      }
      ++min.x;
      Point max = {t.x + 1, t.y};
      while (!used.contains(max) && b.OnBoard(max) && IsDigit(b[max])) {
        used.insert(max);
        ++max.x;
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
