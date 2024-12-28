// http://adventofcode.com/2024/day/10

#include "advent_of_code/2024/day10/day10.h"

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

int Score(const ImmutableCharBoard& b, Point p) {
  absl::flat_hash_set<Point> cur = {p};
  for (int i = 0; i < 9; ++i) {
    absl::flat_hash_set<Point> next;
    for (Point p : cur) {
      for (Point dir : Cardinal::kFourDirs) {
        Point t = p + dir;
        if (b.OnBoard(t) && b[t] == b[p] + 1) {
          next.insert(t);
        }
      }
    }
    cur = std::move(next);
  }
  return cur.size();
}

int Rating(const ImmutableCharBoard& b, Point p) {
  absl::flat_hash_set<std::vector<Point>> cur = {std::vector{p}};
  for (int i = 0; i < 9; ++i) {
    absl::flat_hash_set<std::vector<Point>> next;
    for (const std::vector<Point>& path : cur) {
      for (Point dir : Cardinal::kFourDirs) {
        Point t = path.back() + dir;
        if (b.OnBoard(t) && b[t] == b[path.back()] + 1) {
          std::vector<Point> next_path = path;
          next_path.push_back(t);
          next.insert(next_path);
        }
      }
    }
    cur = std::move(next);
  }
  return cur.size();
}

}  // namespace

absl::StatusOr<std::string> Day_2024_10::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  absl::flat_hash_set<Point> trailheads = b.Find('0');
  int total_score = 0;
  for (Point p : trailheads) {
    total_score += Score(b, p);
  }
  return AdventReturn(total_score);
}

absl::StatusOr<std::string> Day_2024_10::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  absl::flat_hash_set<Point> trailheads = b.Find('0');
  int total_score = 0;
  for (Point p : trailheads) {
    total_score += Rating(b, p);
  }
  return AdventReturn(total_score);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/10,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_10()); });

}  // namespace advent_of_code
