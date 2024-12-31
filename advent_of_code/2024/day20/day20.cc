// http://adventofcode.com/2024/day/20

#include "advent_of_code/2024/day20/day20.h"

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

absl::StatusOr<std::string> Day_2024_20::Part1(
    absl::Span<std::string_view> input) const {
  int min_save;
  if (!absl::SimpleAtoi(param(), &min_save)) return absl::InvalidArgumentError("bad param");
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('S'));
  ASSIGN_OR_RETURN(Point end, b.FindUnique('E'));
  std::vector<Point> path = {start};
  while (path.back() != end) {
    std::optional<Point> found;
    for (Point d : Cardinal::kFourDirs) {
      Point n = path.back() + d;
      if (!b.OnBoard(n)) continue;
      if (b[n] == '#') continue;
      if (path.size() > 1 && n == path[path.size() - 2]) continue;
      if (found) return absl::InvalidArgumentError("unique path");
      found = n;
    }
    if (!found) return absl::InvalidArgumentError("no path");
    path.push_back(*found);
  }
  
  int skips = 0;
  for (int i = 0; i < path.size(); ++i) {
    for (int j = i + min_save + 2; j < path.size(); ++j) {
      if ((path[i] - path[j]).dist() == 2) {
        ++skips;
      }
    }
  }
  return AdventReturn(skips);
}

absl::StatusOr<std::string> Day_2024_20::Part2(
    absl::Span<std::string_view> input) const {
  int min_save;
  if (!absl::SimpleAtoi(param(), &min_save)) return absl::InvalidArgumentError("bad param");
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('S'));
  ASSIGN_OR_RETURN(Point end, b.FindUnique('E'));
  std::vector<Point> path = {start};
  while (path.back() != end) {
    std::optional<Point> found;
    for (Point d : Cardinal::kFourDirs) {
      Point n = path.back() + d;
      if (!b.OnBoard(n)) continue;
      if (b[n] == '#') continue;
      if (path.size() > 1 && n == path[path.size() - 2]) continue;
      if (found) return absl::InvalidArgumentError("unique path");
      found = n;
    }
    if (!found) return absl::InvalidArgumentError("no path");
    path.push_back(*found);
  }
  int skips = 0;
  for (int i = 0; i < path.size(); ++i) {
    for (int j = i + 2; j < path.size(); ++j) {
      int skip_dist = (path[i] - path[j]).dist();
      if (skip_dist > 20) continue;
      int save = j - i - skip_dist;
      if (save >= min_save) {
        ++skips;
      }
    }
  }
  return AdventReturn(skips);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/20,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_20()); });

}  // namespace advent_of_code
