// http://adventofcode.com/2024/day/10

#include "advent_of_code/2024/day10/day10.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/point.h"

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
  absl::flat_hash_map<Point, int> paths_to = {{p, 1}};
  for (int i = 0; i < 9; ++i) {
    absl::flat_hash_map<Point, int> next_to;
    for (const auto& [p, count] : paths_to) {
      for (Point dir : Cardinal::kFourDirs) {
        Point t = p + dir;
        if (b.OnBoard(t) && b[t] == b[p] + 1) {
          next_to[t] += count;
        }
      }
    }
    paths_to = std::move(next_to);
  }
  return absl::c_accumulate(
      paths_to, 0,
      [](int a, std::pair<Point, int> p_and_c) { return a + p_and_c.second; });
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
