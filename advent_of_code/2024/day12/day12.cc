// http://adventofcode.com/2024/day/12

#include "advent_of_code/2024/day12/day12.h"

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

absl::flat_hash_set<Point> Contiguous(
    const ImmutableCharBoard& b, Point p) {
  std::vector<Point> frontier = {p};
  absl::flat_hash_set<Point> ret = {p};
  while (!frontier.empty()) {
    std::vector<Point> new_frontier;
    for (Point p : frontier) {
      for (Point d : Cardinal::kFourDirs) {
        Point t = p + d;
        if (b.OnBoard(t) && b[p] == b[t] && !ret.contains(t)) {
          new_frontier.push_back(t);
          ret.insert(t);
        }
      }
    }
    frontier = std::move(new_frontier);
  }

  return ret;
}

int64_t Score(const absl::flat_hash_set<Point>& set) {
  int area = set.size();
  int perimeter = 0;
  for (Point p : set) {
    for (Point d : Cardinal::kFourDirs) {
      if (!set.contains(p + d)) ++perimeter;
    }
  }
  return area * perimeter;
}

int64_t Score2(const absl::flat_hash_set<Point>& set) {
  int area = set.size();
  int sides = 0;
  for (Point p : set) {
    for (Point d : Cardinal::kFourDirs) {
      if (!set.contains(p + d)) {
        Point ld = d.rotate_left();
        if (!set.contains(p + ld) ||
            set.contains(p + d + ld)) {
          ++sides;
        }
      }
    }
  }
  return area * sides;
}


}  // namespace

absl::StatusOr<std::string> Day_2024_12::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  absl::flat_hash_set<Point> used;
  int total_score = 0;
  for (Point p : b.range()) {
    if (used.contains(p)) continue;
    absl::flat_hash_set<Point> contiguous = Contiguous(b, p);
    for (Point p : contiguous) {
      auto [it, inserted] = used.insert(p);
      if (!inserted) return absl::InternalError("used?");
    }
    total_score += Score(contiguous);
  }
  return AdventReturn(total_score);
}

absl::StatusOr<std::string> Day_2024_12::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  absl::flat_hash_set<Point> used;
  int total_score = 0;
  for (Point p : b.range()) {
    if (used.contains(p)) continue;
    absl::flat_hash_set<Point> contiguous = Contiguous(b, p);
    for (Point p : contiguous) {
      auto [it, inserted] = used.insert(p);
      if (!inserted) return absl::InternalError("used?");
    }
    total_score += Score2(contiguous);
  }
  return AdventReturn(total_score);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/12,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_12()); });

}  // namespace advent_of_code
